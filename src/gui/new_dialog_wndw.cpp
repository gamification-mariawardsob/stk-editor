#include "gui/new_dialog_wndw.hpp"

#include "editor.hpp"


NewDialogWndw* NewDialogWndw::m_self = 0;

// ----------------------------------------------------------------------------
void NewDialogWndw::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    IGUIFont* font = gui_env->getFont(L"font/font1.png");
    dimension2du ss = Editor::getEditor()->getScreenSize();

    rect<s32> frame = rect<s32>(ss.Width  / 2.0f - 150, 
                                ss.Height / 2.0f - 100,
                                ss.Width  / 2.0f + 150,
                                ss.Height / 2.0f + 100);
    m_wndw = gui_env->addWindow(frame,false, L"New Track", 0);

    gui_env->addStaticText(L"Track name:", rect<s32>(10, 30, 130, 50),
        false, false, m_wndw, -1, false)->setOverrideFont(font);
    gui_env->addStaticText(L"Designer:", rect<s32>(10, 60, 130, 80),
        false, false, m_wndw, -1, false)->setOverrideFont(font);
    gui_env->addStaticText(L"File name:", rect<s32>(10, 90, 130, 110),
        false, false, m_wndw, -1, false)->setOverrideFont(font);
    gui_env->addStaticText(L"Size:", rect<s32>(10, 120, 130, 140),
        false, false, m_wndw, -1, false)->setOverrideFont(font);

    frame        =  rect<s32>(150, 30, 280, 50);
    m_track_name =  gui_env->addEditBox(L"", frame, true, m_wndw);
    m_track_name -> setText(L"RenameMePls!");
    frame        =  rect<s32>(150, 60, 280, 80);
    m_designer   =  gui_env->addEditBox(L"", frame, true, m_wndw);
    m_designer   -> setText(L"TheGreatTux");
    frame        =  rect<s32>(150, 90, 280, 110);
    m_file_name  =  gui_env->addEditBox(L"", frame, true, m_wndw);
    m_file_name  -> setText(L"file_name.map");

    frame = rect<s32>(150, 120, 280, 140);
    m_size_cb = gui_env->addComboBox(frame, m_wndw);

    m_size_cb->addItem(L"Small",        50);
    m_size_cb->addItem(L"Medium",       75);
    m_size_cb->addItem(L"Large",        100);
    m_size_cb->addItem(L"Extra Large",  120);

    frame = rect<s32>(120,160,180,180);
    gui_env->addButton(frame, m_wndw, BTN_ID, L"Create", L"Create");

} // init

// ----------------------------------------------------------------------------
NewDialogWndw* NewDialogWndw::get()
{
    if (m_self != 0) return m_self;

    m_self = new NewDialogWndw();
    m_self->init();
    return m_self;
} // getToolBox

// ----------------------------------------------------------------------------
u32 NewDialogWndw::getSize()
{
    return m_size_cb->getItemData(m_size_cb->getSelected());
} // getSize


