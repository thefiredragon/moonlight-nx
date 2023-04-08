//
//  grid_view.hpp
//  Moonlight
//
//  Created by Даниил Виноградов on 03.06.2021.
//

#pragma once

#include <borealis.hpp>

using namespace brls;

class GridView : public Box {
  public:
    GridView();
    GridView(int columns);

    void addView(View* view) override;
    void clearViews(bool free = true) override;
    View* getParentNavigationDecision(View* from, View* newFocus,
                                      FocusDirection direction) override;
    std::vector<View*>& getChildren();
    int getItemIndex(View* view);
    int getRows();
    int getItemsInRow(int row);

  private:
    int columls = 1;
    Box* lastContainer = nullptr;
    View* lastView = nullptr;
    std::vector<View*> children;
};
