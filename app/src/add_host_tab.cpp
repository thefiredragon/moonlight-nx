//
//  add_host_tab.cpp
//  Moonlight
//
//  Created by XITRIX on 26.05.2021.
//

#include "add_host_tab.hpp"
#include "DiscoverManager.hpp"
#include "helper.hpp"
#include "main_tabs_view.hpp"

AddHostTab::AddHostTab() {
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/tabs/add_host.xml");

    hostIP->init("add_host/host_ip"_i18n, "10.0.0.19");

    connect->setText("add_host/connect"_i18n);
    connect->registerClickAction([this](View* view) {
        connectHost(hostIP->getValue());
        return true;
    });

    if (GameStreamClient::instance().can_find_host())
        findHost();
    else {
        searchHeader->setTitle("add_host/search_error"_i18n);
        loader->setVisibility(brls::Visibility::GONE);
    }

    registerAction("add_host/search_refresh"_i18n, ControllerButton::BUTTON_X,
                   [this](View* view) {
#ifdef MULTICAST_DISABLED
                       DiscoverManager::instance().reset();
#endif
                       findHost();
                       return true;
                   });
    setActionAvailable(BUTTON_X, GameStreamClient::instance().can_find_host());
}

void AddHostTab::fillSearchBox(GSResult<std::vector<Host>> hostsRes) {
    loader->setVisibility(DiscoverManager::instance().isPaused()
                              ? brls::Visibility::GONE
                              : brls::Visibility::VISIBLE);

    if (hostsRes.isSuccess()) {
        std::vector<Host> hosts = hostsRes.value();
        for (Host host : hosts) {
            if (searchBoxIpExists(host.address))
                continue;

            brls::DetailCell* hostButton = new brls::DetailCell();
            hostButton->setText(host.hostname);
            hostButton->setDetailText(host.address);
            hostButton->setDetailTextColor(
                brls::Application::getTheme()["brls/text_disabled"]);
            hostButton->registerClickAction([this, host](View* view) {
                connectHost(host.address);
                return true;
            });
            searchBox->addView(hostButton);
        }
    } else {
        loader->setVisibility(brls::Visibility::GONE);
        searchHeader->setTitle("add_host/search"_i18n + " - " +
                               hostsRes.error());
    }
}

bool AddHostTab::searchBoxIpExists(std::string ip) {
    for (View* child : searchBox->getChildren()) {
        DetailCell* cell = dynamic_cast<DetailCell*>(child);
        if (cell->detail->getFullText() == ip)
            return true;
    }
    return false;
}

void AddHostTab::findHost() {
#ifdef MULTICAST_DISABLED
    DiscoverManager::instance().start();
    fillSearchBox(DiscoverManager::instance().getHosts());
    DiscoverManager::instance().getHostsUpdateEvent()->unsubscribe(
        searchSubscription);
    searchSubscription =
        DiscoverManager::instance().getHostsUpdateEvent()->subscribe(
            [this](auto result) { fillSearchBox(result); });
#else
    ASYNC_RETAIN
    GameStreamClient::instance().find_hosts(
        [ASYNC_TOKEN](GSResult<std::vector<Host>> result) {
            ASYNC_RELEASE

            if (result.isSuccess()) {
                std::vector<Host> hosts = result.value();

                for (Host host : hosts) {
                    brls::DetailCell* hostButton = new brls::DetailCell();
                    hostButton->setText(host.hostname);
                    hostButton->setDetailText(host.address);
                    hostButton->setDetailTextColor(
                        brls::Application::getTheme()["brls/text_disabled"]);
                    hostButton->registerClickAction([this, host](View* view) {
                        connectHost(host.address);
                        return true;
                    });
                    searchBox->addView(hostButton);
                }
                loader->setVisibility(brls::Visibility::GONE);
            } else {
                showError(result.error(), [] {});
            }
        });
#endif
}

void AddHostTab::connectHost(std::string address) {
    pauseSearching();

    Dialog* loader = createLoadingDialog("add_host/try_connect"_i18n);
    loader->open();

    GameStreamClient::instance().connect(
        address, [this, loader](GSResult<SERVER_DATA> result) {
            loader->close([this, result] {
                if (result.isSuccess()) {
                    Host host{.address = result.value().address,
                              .hostname = result.value().hostname,
                              .mac = result.value().mac};

                    if (result.value().paired) {
                        showAlert("add_host/paired_error"_i18n, [host] {
                            Settings::instance().add_host(host);
                            MainTabs::getInstanse()->refillTabs();
                        });

                        return;
                    }

                    char pin[5];
                    sprintf(pin, "%d%d%d%d", (int)rand() % 10, (int)rand() % 10,
                            (int)rand() % 10, (int)rand() % 10);

                    brls::Dialog* dialog = createLoadingDialog(
                        "add_host/pair_prefix"_i18n + std::string(pin) +
                        "add_host/pair_postfix"_i18n);
                    dialog->setCancelable(false);
                    dialog->open();

                    ASYNC_RETAIN
                    GameStreamClient::instance().pair(
                        result.value().address, pin,
                        [ASYNC_TOKEN, host, dialog](GSResult<bool> result) {
                            ASYNC_RELEASE
                            dialog->dismiss([this, result, host] {
                                if (result.isSuccess()) {
                                    Settings::instance().add_host(host);
                                    MainTabs::getInstanse()->refillTabs();
                                    this->startSearching();
                                } else {
                                    showError(result.error(), [this] {
                                        this->startSearching();
                                    });
                                }
                            });
                        });
                } else {
                    showError(result.error(),
                              [this] { this->startSearching(); });
                }
            });
        });
}

void AddHostTab::pauseSearching() {
#ifdef MULTICAST_DISABLED
    DiscoverManager::instance().pause();
#endif
}

void AddHostTab::startSearching() {
#ifdef MULTICAST_DISABLED
    DiscoverManager::instance().start();
#endif
}

AddHostTab::~AddHostTab() {
#ifdef MULTICAST_DISABLED
    DiscoverManager::instance().pause();
    DiscoverManager::instance().getHostsUpdateEvent()->unsubscribe(
        searchSubscription);
#endif
}

brls::View* AddHostTab::create() {
    // Called by the XML engine to create a new AddHostTab
    return new AddHostTab();
}
