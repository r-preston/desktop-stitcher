int remove_previous_images()
{
    std::vector<std::string> files;
    std::string buff;
    DIR *dir;
    struct dirent *ent;

    if((dir = opendir("Images/.")) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            files.push_back(ent->d_name);
        }

        closedir(dir);

        for(auto file:files)
        {
            buff = "Images/" + file;
            remove(buff.c_str());
        }
    }
    else
    {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
    return 0;
}

int remove_previous_backgrounds()
{
    std::vector<std::string> files;
    std::string buff;
    DIR *dir;
    struct dirent *ent;

    if((dir = opendir("Background/.")) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            files.push_back(ent->d_name);
        }

        closedir(dir);

        for(auto file:files)
        {
            buff = "Background/" + file;
            remove(buff.c_str());
        }
    }
    else
    {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
    return 0;
}

int remove_profile_images()
{
    std::vector<std::string> files;
    std::string buff;
    DIR *dir;
    struct dirent *ent;

    if((dir = opendir("Profile images/.")) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            files.push_back(ent->d_name);
        }

        closedir(dir);

        for(auto file:files)
        {
            buff = "Profile images/" + file;
            remove(buff.c_str());
        }
    }
    else
    {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
    return 0;
}
