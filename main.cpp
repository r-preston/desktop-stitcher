#ifdef __linux__
    #define SYSTEM_IS_LINUX
#endif
#ifdef _WIN32
    #define SYSTEM_IS_WINDOWS
#endif

#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <dirent.h>
#include <typeinfo>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>

#ifdef SYSTEM_IS_WINDOWS
    #include <windows.h>
    #define IMAGEMAGICK "magick"
    //#define ASK_FOR_PARAMETERS
#endif // SYSTEM_IS_WINDOWS
#ifdef SYSTEM_IS_LINUX
    #include <unistd.h>
    #include <X11/Xlib.h>
    #include <sys/stat.h>
    #include <X11/extensions/Xinerama.h>
    #define IMAGEMAGICK "convert"
#endif // SYSTEM_IS_LINUX

#include "helpers.hpp"
#include "dir_manipulation.hpp"
#include "getinfo.hpp"
#include "extras.hpp"

//============================================================================================================
//============================================================================================================

// X-Windows terminology
//
// a Monitor is a physical device
// a Screen is an X-Window screen (Monitor attached to a Display)
// A Display is a collection of Screens showing parts of a single desktop

std::vector<std::string> matching_images(std::string pattern)
{

    std::vector<std::string> shortlist, file_contents;
    std::vector<Db_Entry> entries;
    std::string line;
    size_t pos;

    std::ifstream readfile;
    readfile.open("Files/tags.txt");

    while(getline(readfile, line))
    {
        file_contents.push_back(line);
    }

    readfile.close();

    for(unsigned int i = 0; i < file_contents.size(); i++)
    {
        Db_Entry entry;
        pos = file_contents[i].find(':');
        entry.file = file_contents[i].substr(0, pos);
        entry.tags = explode(file_contents[i].substr(pos + 1), ',');

        if(entry.file.back() == '\r')
        {
            entry.file.pop_back();
        }
        for(unsigned int j = 0; j < entry.tags.size(); j++)
        {
            if(entry.tags[j].back() == '\r')
            {
                entry.tags[j].pop_back();
            }
        }

        entries.push_back(entry);
    }


    for(unsigned int j = 0; j < entries.size(); j++)
    {
        if(pattern == "any")
        {
            shortlist.push_back(entries[j].file);
            continue;
        }
        if(pattern == entries[j].file)
        {
            shortlist.push_back(entries[j].file);
            continue;
        }
        if( std::find(entries[j].tags.begin(), entries[j].tags.end(), pattern) != entries[j].tags.end() )
        {
            shortlist.push_back(entries[j].file);
        }
    }

    return shortlist;
}

int create_background(std::string image, int img_x, int img_y, Monitor &screen, int screen_no)
{
    std::string imagemagick(IMAGEMAGICK);

    std::string command;
    std::string screen_size = s(screen.x) + "x" + s(screen.y);

    int x_offset = 0;
    int y_offset = 0;
    if((double(img_x)/double(img_y)) < (double(screen.x)/double(screen.y)))
    {
        int yprime = img_y*(double(screen.x)/double(img_x));
        y_offset = (yprime-screen.y)/2;
    }
    else
    {
        int xprime = img_x*(double(screen.y)/double(img_y));
        x_offset = (xprime-screen.x)/2;
    }

    command = imagemagick + " \"" + image + "\" -resize " + screen_size + "^ -crop " + screen_size
              + "+" + s(x_offset) + "+" + s(y_offset) + " Images/" + s(screen_no) + ".jpg";


    runprocess(command);

    // adjust images to account for vertical alignment

    // if the top of a monitor is lower than that of the main monitor
    if(screen.top_offset > 0)
    {
        // create vertical buffer image
        command = imagemagick + " -size " + s(screen.x) + "x" + s(screen.top_offset) + " xc:white Images/buffer.jpg";
        runprocess(command);

        // attach it to the top of the image
        command = imagemagick + " Images/buffer.jpg Images/" + s(screen_no) + ".jpg -append Images/" + s(screen_no) + ".jpg";
        runprocess(command);

        remove("Images/buffer.jpg");
    }
    return 0;
}

int create_spanning_background(std::string image, int img_x, int img_y, int dsp_x, int dsp_y, int screens)
{
    std::string imagemagick(IMAGEMAGICK);
    std::string command;

    Monitor virtual_monitor(dsp_x, dsp_y, 0, 0);

    create_background(image, img_x, img_y, virtual_monitor, 0);

    for(unsigned int i = 1; i < screens; i++)
    {
        command = imagemagick + " Files/empty.jpg Images/" + s(i) + ".jpg";
        runprocess(command);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    std::string imagemagick(IMAGEMAGICK);

    std::vector<std::string> filelist;
    std::vector<char*> arg_list;
    std::vector<Monitor> screens = get_screen_configuration();
    srand(time(0));
    std::vector<std::string> arguments;
    std::string newname, command;

    for(int i = 1; i < argc; i++)
    {
        arg_list.push_back(argv[i]);
    }

    #ifdef ASK_FOR_PARAMETERS
        std::string user_input;
        std::vector<std::string> arguments;
        std::cin >> user_input;
        arguments = explode(user_input, ' ');
        for(auto ca:arguments)
        {
            char *item = &ca[0u];
            arg_list.push_back(item);
        }
        for(auto n:arg_list)
            std::cout << n << std::endl;
    #endif // ASK_FOR_PARAMETERS

    if(get_images(filelist))
    {
        std::cout << "Error: could not access directory './'" << std::endl;
        return 1;
    }
    if(filelist.size() == 0)
    {
        std::cout << "No image files found" << std::endl;
        return 1;
    }

    if(arg_list.size())
    {

        if(!strcmp(arg_list[0], "-h"))
        {
            std::cout << "\nDesktopStitcher help\n\n" << std::endl;

            std::cout << "  Optional arguments:\n" << std::endl;

            std::cout << "    DesktopStitcher [TAG NAME/IMAGE NAME]:[COUNT] ... " << std::endl;
            std::cout << "    Updates the image tag database (tags.txt) located in /Files" << std::endl;
            std::cout << std::endl;

            std::cout << "    DesktopStitcher -updb, -u" << std::endl;
            std::cout << "    Updates the image tag database (tags.txt) located in /Files" << std::endl;
            std::cout << std::endl;

            std::cout << "    DesktopStitcher -profpics, -p (Windows only)" << std::endl;
            std::cout << "    Creates profile images in the directory '/Profile images' such that the profile picture will be camouflaged against its corresponding image on the Windows login screen" << std::endl;
            std::cout << std::endl;
            return 0;
        }
        if(!( strcmp(arg_list[0], "-updb") && strcmp(arg_list[0], "-u") ))
        {
            update_tag_database(filelist);
            return 0;
        }
        if(!( strcmp(arg_list[0], "-profpics") && strcmp(arg_list[0], "-p") ))
        {
            return build_profile_images(filelist);
        }

        std::string arg;
        size_t pos;

        update_tag_database(filelist);
        for(unsigned int i = 0; i < arg_list.size(); i++)
        {
            arg = arg_list[i];
            pos = arg.find(':');
            if(pos == std::string::npos)
            {
                arg += ":";
                arg += s(screens.size());
                pos = arg.find(':');
            }
            if(pos == arg.size() - 1)
            {
                arg += s(screens.size());
                pos = arg.find(':');
            }
            if(!is_integer(arg.substr(pos + 1)))
            {
                std::cout << "Malformed argument '" << arg << "' (should be in form [TAG/NAME]:[COUNT])" << std::endl;
                return 1;
            }
            for(unsigned int j = 0; j < std::stoi(arg.substr(pos + 1)); j++)
            {
                arguments.push_back(arg.substr(0, pos));
            }
        }

    }

    #ifdef SYSTEM_IS_WINDOWS
        CreateDirectory("Background", NULL);
        CreateDirectory("Images", NULL);
    #endif // SYSTEM_IS_WINDOWS
    #ifdef SYSTEM_IS_LINUX
        mkdir("Images", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    #endif // SYSTEM_IS_LINUX
    #ifdef SYSTEM_IS_WINDOWS
        std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        for(int i = 0; i < 20; i++)
        {
            newname += chars[rand() % chars.size()];
        }
        newname = "Background/" + newname + ".png";
        remove_previous_backgrounds();
    #endif // SYSTEM_IS_WINDOWS
    remove_previous_images();
    #ifdef SYSTEM_IS_LINUX
        newname = "Images/background.png";
    #endif // SYSTEM_IS_LINUX

    bool using_specified_images = false;
    for(int i = 0; i < screens.size(); i++)
    {
        std::string image;
        std::vector<int> img_size;
        if(i < arguments.size())
        {
            filelist = matching_images(arguments[i]);
            if(filelist.size() == 0)
            {
                get_images(filelist);
            }
            else
            {
                using_specified_images = true;
            }
        }
        else
        {
            get_images(filelist);
        }

        do {
            if(filelist.size() == 0)
            {
                if(using_specified_images)
                {
                    get_images(filelist);
                    using_specified_images = false;
                }
                else
                {
                    std::cout << "No images large enough" << std::endl;
                    return 1;
                }
            }
            int position = rand() % filelist.size();
            image = filelist[position];
            filelist.erase(filelist.begin() + position);
            img_size = get_image_size(image);
        } while((img_size[0] < screens[i].x) || (img_size[1] < screens[i].y));

        // if spanning allowed
        if(((i == 0) && (arguments.size() < 2)) || (std::find(arguments.begin(), arguments.end(), "-span") != arguments.end()))
        {
            // is the image large enough?
            std::vector<int> vspace, hspace;
            for(int j = 0; j < screens.size(); j++)
            {
                /*std::cout << "Screen " << j << ": ";
                screens[j].print();*/
                hspace.push_back(screens[j].position + screens[j].x);
                vspace.push_back(screens[j].top_offset + screens[j].y);
            }
            int display_area_x = maximum(hspace);
            int display_area_y = maximum(vspace);

            if(!((img_size[0] < display_area_x) || (img_size[1] < display_area_y)))
            {
                if((rand() % 100 < 40) || (std::find(arguments.begin(), arguments.end(), "-span") != arguments.end()))
                {
                    create_spanning_background(image, img_size[0], img_size[1], display_area_x, display_area_y, screens.size());
                    break;
                }
            }
        }
        create_background(image, img_size[0], img_size[1], screens[i], i);
    }

    // stitch images together into one super image
    command = imagemagick + " ";
    for(unsigned int i = 0; i < screens.size(); i++)
    {
        command += "Images/" + s(i) + ".jpg ";
    }
    command += "+append " + newname;

    runprocess(command);

    return 0;
}
