int build_profile_images(std::vector<std::string> images)
{
    // building profile images works only for 1920x1080 pixel images
    // it's really just a convenience created for me rather than a true feature

    std::string command;

    std::string imagemagick(IMAGEMAGICK);

    #ifdef SYSTEM_IS_WINDOWS
    CreateDirectory("Profile images", NULL);
    #endif // SYSTEM_IS_WINDOWS
    #ifdef SYSTEM_IS_LINUX
    mkdir("Profile images", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    #endif // SYSTEM_IS_LINUX

    remove_profile_images();

    command = imagemagick + " -size 188x188 xc:black Images/overlay.png";
    runprocess(command);

    for(auto hd_background:images)
    {
        std::vector<int> img_size = get_image_size(hd_background);
        if(img_size[0] != 1920) continue;
        if(img_size[1] != 1080) continue;

        size_t pos = hd_background.find_last_of('.');
        std::string img = "\"Profile images/" + hd_background.substr(0,pos) + " profile.jpg\"";

        command = imagemagick + " \"" + hd_background + "\" -crop 188x188+866+317 " + img;
        runprocess(command);

        #ifdef SYSTEM_IS_LINUX
        command = "composite -blend 45 \"Images/overlay.png\" " + img + " " + img;
        #endif
        #ifdef SYSTEM_IS_WINDOWS
        command = imagemagick + " composite -blend 45 \"Images/overlay.png\" " + img + " " + img;
        #endif
        runprocess(command);
    }
    return 0;
}

int update_tag_database(std::vector<std::string> filelist)
{
    #ifdef SYSTEM_IS_WINDOWS
    CreateDirectory("Files", NULL);
    #endif // SYSTEM_IS_WINDOWS
    #ifdef SYSTEM_IS_LINUX
    mkdir("Files", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    #endif // SYSTEM_IS_LINUX

    std::vector<std::string> namepart, tagpart;
    std::string line;

    std::ifstream readfile;
    readfile.open("Files/tags.txt");

    while(getline(readfile, line))
    {
        size_t pos = line.find(':');
        namepart.push_back(line.substr(0,pos));
        tagpart.push_back(line.substr(pos));
    }

    // add new files
    for(unsigned int i = 0; i < filelist.size(); i++)
    {
        if(std::find(namepart.begin(), namepart.end(), filelist[i]) == namepart.end())
        {
            namepart.push_back(filelist[i]);
            tagpart.push_back(":");
        }
    }
    // remove non-existent files
    for(unsigned int i = 0; i < namepart.size(); i++)
    {
        if(std::find(filelist.begin(), filelist.end(), namepart[i]) == filelist.end())
        {
            namepart.erase(namepart.begin() + i);
            tagpart.erase(tagpart.begin() + i);
            i--;
        }
    }

    std::ofstream writefile;
    writefile.open("Files/tags.txt", std::ofstream::trunc | std::ofstream::binary | std::ofstream::out);

    for(unsigned int i = 0; i < namepart.size(); i++)
    {
        if(tagpart[i] == ":") tagpart[i] = ":new";
        writefile << namepart[i] << tagpart[i];
        if(i != namepart.size() - 1)
            writefile << '\n';
    }

    writefile.close();

    return 0;
}
