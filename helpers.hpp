struct Db_Entry
{
    public:
        std::string file;
        std::vector<std::string> tags;
};

struct Monitor
{
    public:
        int x,y, top_offset, position;
        Monitor();
        Monitor(int, int, int, int);
        void print();
};

void Monitor::print()
{
    std::cout << this->x << "x" << this->y << " at (" << this->position << ", " << this->top_offset << ")" << std::endl;
}

int maximum(std::vector<int> v)
{
    if(v.size() == 0) return 0;
    int m = v[0];
    for(unsigned int i = 1; i < v.size(); i++)
    {
        if(v[i] > m) m = v[i];
    }
    return m;
}

Monitor::Monitor(int width, int height, int top, int left)
{
    this->x = width;
    this->y = height;
    this->top_offset = top;
    this->position = left;
}

inline std::string s(int n)
{
    return std::to_string(n);
}

int runprocess(std::string command)
{
    int rc;

    #ifdef SYSTEM_IS_WINDOWS
    char *magick = &command[0u];

    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof si ;

    rc = CreateProcess(NULL, magick, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    WaitForSingleObject( pi.hProcess, INFINITE ); // wait for process to complete
    CloseHandle( pi.hProcess ); // remove handle to process
    CloseHandle( pi.hThread ); // remove thread
    #endif
    #ifdef SYSTEM_IS_LINUX
    // why can't YOU be this simple, Windows?
    rc = system(command.c_str());
    #endif // SYSTEM_IS_LINUX

    return rc;
}

std::vector<std::string> explode(std::string s, char c)
{
    /*
    splits a string on c
    for example, explode("list,of,words",',') returns {"list","of","words"}
    function ignores any instances of c inside double quotes
    */

	std::string buffer; // stores current word
	std::vector<std::string> v; // output
	bool track = true; // keeps track of whether the loop is in a double quote pair or not

	for(auto n:s) // iterate through s
	{
	    if(n == '"')
            track = !track; // we have just entered/left a quote pair
	    if(!track)
        {
            buffer += n; // if in quote pair, add n to buffer regardless
            continue; // skip any further comparisons
        }
		if(n != c)
            buffer += n; // if n is not c, add to buffer
        else if(n == c && buffer != "" && track)
        {
            v.push_back(buffer); // if n is c, add buffer to output
            buffer = ""; // reset buffer
        }
	}
	if(buffer !=  "") // if there is something in the buffer
        v.push_back(buffer); // add whatever buffer is to output

	return v;
}

bool is_integer(const std::string s, bool allow_void = true)
{
    /*
    checks if s contains only numbers, i.e. s is an integer
    by default, '~' is accepted, which is translated as NULL
    */
    if(s == "~" && allow_void) return true;
    std::string digits = "0123456789";
    for(auto n:s) // iterate through s
        if(digits.find(n) == std::string::npos) // if n is not a number
            return false;
    return true; // s is a number
}
