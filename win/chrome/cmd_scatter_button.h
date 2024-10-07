#ifndef CMD_SCATTER_BUTTON_H
#define CMD_SCATTER_BUTTON_H

#include "base/utf_string_conversions.h"

#include "uigfx/image/image.h"

#include "uibase/models/simple_menu_model.h"
#include "uibase/resource/app_res_ids.h"
#include "uibase/resource/resource_bundle.h"
#include "uibase/theme_provider.h"

#include "uiview/controls/button/button_dropdown.h"
#include "uiview/controls/button/checkbox.h"
#include "uiview/controls/button/image_button.h"
#include "uiview/controls/button/menu_button.h"
#include "uiview/controls/button/radio_button.h"
#include "uiview/controls/button/text_button.h"
#include "uiview/controls/menu/menu_2.h"
#include "uiview/controls/menu/view_menu_delegate.h"
#include "uiview/layout/box_layout.h"

class CmdScatterMenuButton;
class CmdScatterMenuModel : public ui::SimpleMenuModel,
    public ui::SimpleMenuModel::Delegate
{
public:
    CmdScatterMenuModel(CmdScatterMenuButton* button);

    void RunMenuAt(const gfx::Point& point);

    // Overridden from ui::SimpleMenuModel::Delegate:
    virtual bool IsCommandIdChecked(int command_id) const;
    virtual bool IsCommandIdEnabled(int command_id) const;
    virtual bool GetAcceleratorForCommandId(
        int command_id,
        ui::Accelerator* accelerator);
    virtual void ExecuteCommand(int command_id);

private:
    enum
    {
        kGroupMakeDecision,
    };
    CmdScatterMenuButton* button_;
    scoped_ptr<view::Menu2> menu_;

    DISALLOW_COPY_AND_ASSIGN(CmdScatterMenuModel);
};

class CmdScatterMenuButton : public view::MenuButton,
    public view::ViewMenuDelegate
{
public:
    CmdScatterMenuButton(ui::ThemeProvider* tp, bool show_menu_marker);
    virtual ~CmdScatterMenuButton();

    void updateIcon();

private:
    // Overridden from views::ViewMenuDelegate:
    virtual void RunMenu(view::View* source, const gfx::Point& point);

    scoped_ptr<CmdScatterMenuModel> menu_model_;
    ui::ThemeProvider* tp_;
    DISALLOW_COPY_AND_ASSIGN(CmdScatterMenuButton);
};


#endif /*CMD_SCATTER_BUTTON_H */

