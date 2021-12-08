#ifdef SYSTEM_IS_WINDOWS
BOOL CALLBACK get_monitor_info(HMONITOR hMonitor, HDC hdcMonitor, LPRECT monRect, LPARAM dwData)
{
    std::vector<Monitor> * pScreens = (std::vector<Monitor>*)dwData;

    int width = monRect->right - monRect->left;
    int height = monRect->bottom - monRect->top;

    Monitor screen(width, height, monRect->top, monRect->left);

    pScreens->push_back(screen);

    return TRUE;

}
#endif

std::vector<Monitor> get_screen_configuration()
{
    std::vector<Monitor> screens, sorted_screens;
    int pos = 0, mem = 0;

    #ifdef SYSTEM_IS_WINDOWS

    EnumDisplayMonitors(NULL, NULL, get_monitor_info, (LPARAM)&screens);

    #endif

    #ifdef SYSTEM_IS_LINUX

    Display *xDisplay = XOpenDisplay(NULL);

    int screen_count;

    Monitor xMonitor(0,0,0,0);

    XineramaScreenInfo * XinScreens = XineramaQueryScreens(xDisplay, &screen_count);

    for(int i = 0; i < screen_count; i++)
    {
        xMonitor.x = XinScreens[i].width;
        xMonitor.y = XinScreens[i].height;
        xMonitor.position = XinScreens[i].x_org;
        xMonitor.top_offset = XinScreens[i].y_org;

        screens.push_back(xMonitor);
    }

    XFree(XinScreens);

    #endif // SYSTEM_IS_LINUX

    for(unsigned int i = 0; i < screens.size(); i++)
    {
        if(screens[i].top_offset < mem)
            mem = screens[i].top_offset;
    }

    for(unsigned int i = 0; i < screens.size(); i++)
    {
        screens[i].top_offset -= mem;
    }

    mem = 0;

    while(screens.size() > 0)
    {
        pos = 0;
        mem = screens[pos].position;
        for(unsigned int i = 0; i < screens.size(); i++)
        {
            if(screens[i].position < mem)
            {
                mem = screens[i].position;
                pos = i;
            }
        }
        sorted_screens.push_back(screens[pos]);
        screens.erase(screens.begin() + pos);
    }

    return sorted_screens;
}

int get_images(std::vector<std::string> &files, int screens = 1)
{
    DIR *dir;
    struct dirent *ent;

    files.clear();

    if((dir = opendir("./")) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            std::string name = ent->d_name;
            size_t pos = name.find_last_of('.');

            if(pos == std::string::npos) continue;

            if(name.substr(pos) == ".png" || name.substr(pos) == ".jpg")
                files.push_back(name);
        }
        closedir(dir);
    }
    else
    {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
    return 0;
}

std::vector<int> get_image_size(std::string path)
{
    std::string imagemagick(IMAGEMAGICK);
    std::string result;
    std::string command = "identify -ping -format %wx%h \"" + path +"\"";

    #ifdef SYSTEM_IS_WINDOWS
        char *magick = &command[0u];
        std::array<char, 256> buffer;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(magick, "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
    #endif
    #ifdef SYSTEM_IS_LINUX
        std::array<char, 128> buffer;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe)
        {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }
    #endif // SYSTEM_IS_LINUX

    size_t pos = result.find("x");
    int width = std::stoi(result.substr(0,pos));
    int height = std::stoi(result.substr(pos + 1));

    return {width, height};
}
