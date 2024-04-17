#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Ultralight/CAPI.h>
#include <AppCore/CAPI.h>
#pragma comment(lib, "Ultralight.lib")
#pragma comment(lib, "AppCore.lib")
#pragma comment(lib, "WebCore.lib")
#pragma comment(lib, "UltralightCore.lib")


#include <Ultralight/Defines.h>
#include <Ultralight/KeyCodes.h>
#include <Ultralight/String.h>
#include <Ultralight/KeyEvent.h>
#include <Ultralight/View.h>


#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

using namespace ultralight;


class MyApp : public LoadListener,
    public Logger {
    RefPtr<Renderer> renderer_;
    RefPtr<View> view_;
    bool done_ = false;
public:
    MyApp() {


        Config config;
        Platform::instance().set_config(config);
        Platform::instance().set_font_loader(GetPlatformFontLoader());
        Platform::instance().set_file_system(GetPlatformFileSystem("./assets/"));
        Platform::instance().set_logger(this);
        renderer_ = Renderer::Create();
        ViewConfig view_config;
        view_config.initial_device_scale = 1.0;
        view_config.is_accelerated = false;
        view_ = renderer_->CreateView(1600, 800, view_config, nullptr);
        view_->set_load_listener(this);

        //view_->LoadURL("https://keyjs.dev/");
        view_->LoadURL("https://www.coderstool.com/key-event-viewer");
   
    }

    virtual ~MyApp() {
        view_ = nullptr;
        renderer_ = nullptr;
    }

    void Run() {
        LogMessage(LogLevel::Info, "Starting Run(), waiting for page to load...");

        do {
            renderer_->Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (!done_);

        renderer_->Render();

        view_->Focus();


        //wait 3seconds
        int i = 0;
        do {
            renderer_->Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            i++;
        } while (i < 300);

        view_->Focus();

        //fire key
        ultralight::KeyEvent keyEvent;
        keyEvent.type = ultralight::KeyEvent::kType_RawKeyDown;
        keyEvent.virtual_key_code = ultralight::KeyCodes::GK_E;
        keyEvent.native_key_code = 0;
        keyEvent.modifiers = 0;
        ultralight::GetKeyIdentifierFromVirtualKeyCode(keyEvent.virtual_key_code, keyEvent.key_identifier);
        view_->FireKeyEvent(keyEvent);


        // Synthesize an  event for text generated from pressing the 'A' key
        KeyEvent evt;
        evt.type = KeyEvent::kType_Char;
        evt.text = "E";
        evt.unmodified_text = "E"; // If not available, set to same as evt.text
        view_->FireKeyEvent(evt);


        //wait 3 seconds
        i = 0;
        do {
            renderer_->Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            i++;
        } while (i < 300);


        BitmapSurface* bitmap_surface = (BitmapSurface*)view_->surface();

        RefPtr<Bitmap> bitmap = bitmap_surface->bitmap();

        bitmap->WritePNG("result.png");

        LogMessage(LogLevel::Info, "Saved a render of our page to result.png.");
    }
    virtual void OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
        const String& url) override {
        if (is_main_frame) {
            LogMessage(LogLevel::Info, "Our page has loaded!");
            done_ = true;
        }
    }

    virtual void LogMessage(LogLevel log_level, const String& message) override {
        std::cout << "> " << message.utf8().data() << std::endl << std::endl;
    }
};

int main() {
    MyApp app;
    app.Run();
    return 0;
}
