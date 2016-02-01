#include <stdio.h>
#include <iostream>
#include <sstream>

#include <SDL.h>

#include <cef_app.h>
#include <cef_client.h>
#include <cef_render_handler.h>
#include <cef_life_span_handler.h>
#include <wrapper/cef_helpers.h>

#include "sdl_keyboard_utils.h"

class RenderHandler :
    public CefRenderHandler
{
public:
    RenderHandler(SDL_Renderer * renderer, int w, int h) :
        width(w),
        height(h),
        renderer(renderer)
    {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, w, h);
    }

    ~RenderHandler()
    {
        if (texture)
        {
            SDL_DestroyTexture(texture);
        }
        renderer = nullptr;
    }

    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
    {
        rect = CefRect(0, 0, width, height);
        return true;
    }

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void * buffer, int w, int h)
    {
        if (texture)
        {
            unsigned char * texture_data = NULL;
            int texture_pitch = 0;

            SDL_LockTexture(texture, 0, (void **)&texture_data, &texture_pitch);
            memcpy(texture_data, buffer, w * h * 4);
            SDL_UnlockTexture(texture);
        }
    }

    void resize(int w, int h)
    {
        if (texture)
        {
            SDL_DestroyTexture(texture);
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, w, h);
        width = w;
        height = h;
    }

    void render()
    {
        SDL_RenderCopy(renderer, texture, NULL, NULL);
    }

private:
    int width;
    int height;
    SDL_Renderer * renderer = nullptr;
    SDL_Texture * texture = nullptr;

    IMPLEMENT_REFCOUNTING(RenderHandler);
};

// for manual render handler
class BrowserClient :
    public CefClient,
    public CefLifeSpanHandler
{
public:
    BrowserClient(CefRefPtr<CefRenderHandler> ptr) :
        handler(ptr)
    {
    }

    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler()
    {
        return this;
    }

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler()
    {
        return handler;
    }

    // CefLifeSpanHandler methods.
    void OnAfterCreated(CefRefPtr<CefBrowser> browser)
    {
        // Must be executed on the UI thread.
        CEF_REQUIRE_UI_THREAD();

        browser_id = browser->GetIdentifier();
    }

    bool DoClose(CefRefPtr<CefBrowser> browser)
    {
        // Must be executed on the UI thread.
        CEF_REQUIRE_UI_THREAD();

        // Closing the main window requires special handling. See the DoClose()
        // documentation in the CEF header for a detailed description of this
        // process.
        if (browser->GetIdentifier() == browser_id)
        {
            // Set a flag to indicate that the window close should be allowed.
            closing = true;
        }

        // Allow the close. For windowed browsers this will result in the OS close
        // event being sent.
        return false;
    }

    void OnBeforeClose(CefRefPtr<CefBrowser> browser)
    {
    }

    bool closeAllowed() const
    {
        return closing;
    }

private:
    int browser_id;
    bool closing = false;
    CefRefPtr<CefRenderHandler> handler;

    IMPLEMENT_REFCOUNTING(BrowserClient);
};

CefBrowserHost::MouseButtonType translateMouseButton(SDL_MouseButtonEvent const &e)
{
    CefBrowserHost::MouseButtonType result;
    switch (e.button)
    {
        case SDL_BUTTON_LEFT:
        case SDL_BUTTON_X1:
            result = MBT_LEFT;
            break;

        case SDL_BUTTON_MIDDLE:
            result = MBT_MIDDLE;
            break;

        case SDL_BUTTON_RIGHT:
        case SDL_BUTTON_X2:
            result = MBT_RIGHT;
            break;
    }
    return result;
}

int main(int argc, char * argv[])
{
    CefMainArgs args;

    {
        int result = CefExecuteProcess(args, nullptr, nullptr);
        // checkout CefApp, derive it and set it as second parameter, for more control on
        // command args and resources.
        if (result >= 0) // child proccess has endend, so exit.
        {
            return result;
        }
        else if (result == -1)
        {
            // we are here in the father proccess.
        }
    }

    {
        CefSettings settings;

        // CefString(&settings.resources_dir_path).FromASCII("");
        {
            std::ostringstream ss;
            ss << SDL_GetBasePath() << "locales/";
            CefString(&settings.locales_dir_path) = ss.str();
        }

        // CefString(&settings.locales_dir_path).FromASCII("");
        CefString(&settings.resources_dir_path) = SDL_GetBasePath();

        // checkout detailed settings options http://magpcss.org/ceforum/apidocs/projects/%28default%29/_cef_settings_t.html
        // nearly all the settings can be set via args too.
        // settings.multi_threaded_message_loop = true; // not supported, except windows
        // CefString(&settings.browser_subprocess_path).FromASCII("sub_proccess path, by default uses and starts this executeable as child");
        // CefString(&settings.cache_path).FromASCII("");
        // CefString(&settings.log_file).FromASCII("");
        // settings.log_severity = LOGSEVERITY_DEFAULT;

        bool result = CefInitialize(args, settings, nullptr, nullptr);
        // CefInitialize creates a sub-proccess and executes the same executeable, as calling CefInitialize, if not set different in settings.browser_subprocess_path
        // if you create an extra program just for the childproccess you only have to call CefExecuteProcess(...) in it.
        if (!result)
        {
            // handle error
            return -1;
        }
    }

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    int width = 800;
    int height = 600;

    auto window = SDL_CreateWindow("Render CEF with SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
    if (window)
    {
        auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer)
        {
            SDL_Event e;

            CefRefPtr<RenderHandler> renderHandler = new RenderHandler(renderer, width, height);

            // create browser-window
            CefRefPtr<CefBrowser> browser;
            CefRefPtr<BrowserClient> browserClient;

            {
                CefWindowInfo window_info;
                CefBrowserSettings browserSettings;

                // browserSettings.windowless_frame_rate = 60; // 30 is default

                window_info.SetAsWindowless(NULL, false); // false means no transparency (site background colour)

                browserClient = new BrowserClient(renderHandler);

                browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(), "http://www.google.com", browserSettings, nullptr);

                // inject user-input by calling - non-trivial for non-windows - checkout the cefclient source and the platform specific cpp, like cefclient_osr_widget_gtk.cpp for linux
                // browser->GetHost()->SendKeyEvent(...);
                // browser->GetHost()->SendMouseMoveEvent(...);
                // browser->GetHost()->SendMouseClickEvent(...);
                // browser->GetHost()->SendMouseWheelEvent(...);
            }

            bool shutdown = false;
            while (!browserClient->closeAllowed())
            {
                // send events to browser
                while (!shutdown && SDL_PollEvent(&e) != 0)
                {
                    switch (e.type)
                    {
                        case SDL_QUIT:
                            shutdown = true;
                            browser->GetHost()->CloseBrowser(false);
                            break;

                        case SDL_KEYDOWN:
                            {
                                CefKeyEvent event;
                                event.modifiers = getKeyboardModifiers(e.key.keysym.mod);
                                event.windows_key_code = getWindowsKeyCode(e.key.keysym);

                                event.type = KEYEVENT_RAWKEYDOWN;
                                browser->GetHost()->SendKeyEvent(event);

                                event.type = KEYEVENT_CHAR;
                                browser->GetHost()->SendKeyEvent(event);
                            }
                            break;

                        case SDL_KEYUP:
                            {
                                CefKeyEvent event;
                                event.modifiers = getKeyboardModifiers(e.key.keysym.mod);
                                event.windows_key_code = getWindowsKeyCode(e.key.keysym);

                                event.type = KEYEVENT_KEYUP;

                                browser->GetHost()->SendKeyEvent(event);
                            }
                            break;

                        case SDL_WINDOWEVENT:
                            switch (e.window.event)
                            {
                                case SDL_WINDOWEVENT_SIZE_CHANGED:
                                    renderHandler->resize(e.window.data1, e.window.data2);
                                    browser->GetHost()->WasResized();
                                    break;

                                case SDL_WINDOWEVENT_FOCUS_GAINED:
                                    browser->GetHost()->SetFocus(true);
                                    break;

                                case SDL_WINDOWEVENT_FOCUS_LOST:
                                    browser->GetHost()->SetFocus(false);
                                    break;

                                case SDL_WINDOWEVENT_HIDDEN:
                                case SDL_WINDOWEVENT_MINIMIZED:
                                    browser->GetHost()->SetWindowVisibility(false);
                                    browser->GetHost()->WasHidden(true);
                                    break;

                                case SDL_WINDOWEVENT_SHOWN:
                                case SDL_WINDOWEVENT_RESTORED:
                                    browser->GetHost()->SetWindowVisibility(true);
                                    browser->GetHost()->WasHidden(false);
                                    break;

                                case SDL_WINDOWEVENT_CLOSE:
                                    e.type = SDL_QUIT;
                                    SDL_PushEvent(&e);
                                    break;
                            }
                            break;

                        case SDL_MOUSEMOTION:
                            {
                                CefMouseEvent event;
                                event.x = e.motion.x;
                                event.y = e.motion.y;

                                browser->GetHost()->SendMouseMoveEvent(event, false);
                            }
                            break;

                        case SDL_MOUSEBUTTONUP:
                            {
                                CefMouseEvent event;
                                event.x = e.button.x;
                                event.y = e.button.y;

                                browser->GetHost()->SendMouseClickEvent(event, translateMouseButton(e.button), true, 1);
                            }
                            break;

                        case SDL_MOUSEBUTTONDOWN:
                            {
                                CefMouseEvent event;
                                event.x = e.button.x;
                                event.y = e.button.y;

                                browser->GetHost()->SendMouseClickEvent(event, translateMouseButton(e.button), false, 1);
                            }
                            break;

                        case SDL_MOUSEWHEEL:
                            {
                                int delta_x = e.wheel.x;
                                int delta_y = e.wheel.y;

                                if (SDL_MOUSEWHEEL_FLIPPED == e.wheel.direction)
                                {
                                    delta_y *= -1;
                                }
                                else
                                {
                                    delta_x *= -1;
                                }

                                CefMouseEvent event;
                                browser->GetHost()->SendMouseWheelEvent(event, delta_x, delta_y);
                            }
                            break;
                    }
                }

                // let browser process events
                CefDoMessageLoopWork();

                // render
                SDL_RenderClear(renderer);

                renderHandler->render();

                // Update screen
                SDL_RenderPresent(renderer);
            }

            browser = nullptr;
            browserClient = nullptr;
            renderHandler = nullptr;

            CefShutdown();

            SDL_DestroyRenderer(renderer);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}