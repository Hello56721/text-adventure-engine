#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <fstream>

#include <cstdint>

using namespace std::literals::string_literals;

#define TRACE_LOG
#ifdef TRACE_LOG
    #define TRACE(x) std::cout << "[TRACE]: " << x << std::endl
#else
    #define TRACE(x)
#endif

namespace
{
    enum class passage_type_t : uint8_t
    {
        NORMAL,
        END
    };

    struct option_t
    {
        std::string label;
        std::string link;
    };

    struct passage_t
    {
        passage_type_t type;
        std::string text;
        std::vector<option_t> options;
    };
    
    struct field_t
    {
        std::string name;
        std::string value;
    };
    
    field_t process_field(std::string_view p_field_string)
    {
        field_t result;
        
        bool reading_name = true;
        
        for (auto character = p_field_string.begin(); character != p_field_string.end(); character++)
        {
            if (*character == '=')
            {
                reading_name = false;
            }
            else 
            {
                if (reading_name)
                {
                    result.name += *character;
                }
                else 
                {
                    result.value += *character;
                }
            }
        }
        
        return result;
    }
    
    std::vector<option_t> process_options(std::string_view array_string)
    {
        std::vector<option_t> result;
        
        option_t current_option;
        bool reading_name = true;
        
        for (auto character = array_string.begin(); character != array_string.end(); character++)
        {
            if (*character == '[' || *character == ']' || *character == '\n' || *character == '\t')
            {
                continue;
            }
            
            if ((*character == ' ') && character != array_string.begin() && *(character - 1) == ' ')
            {
                continue;
            }
            
            if (*character == '|')
            {
                result.push_back(current_option);
                current_option.label = "";
                current_option.link = "";
                reading_name = true;
            }
            else if (*character == ':')
            {
                reading_name = false;
            }
            else 
            {
                if (reading_name)
                {
                    current_option.label += *character;
                }
                else 
                {
                    current_option.link += *character;
                }
            }
        }
        
        for (auto& option: result)
        {
            if (option.label[0] == ' ')
            {
                option.label.erase(0, 1);
            }
            if (option.link[0] == ' ')
            {
                option.link.erase(0, 1);
            }
        }
        
        return result;
    }
    
    passage_t load_passage(std::string_view p_path)
    {
        auto path = "game/"s + p_path.data();
        
        auto file{std::ifstream(path.data())};
        if (!file.is_open())
        {
            std::cerr << "[ERROR]: Failed to open " << path << std::endl;
            return {};
        }
        
        passage_t result;
        
        while (file)
        {
            auto field_string{""s};
            std::getline(file, field_string, ';');
            
            auto field = process_field(field_string);
            if (field.name[0] == '\n')
            {
                field.name.erase(0, 1);
            }
            
            if (field.name == "type")
            {
                if (field.value == "NORMAL")
                {
                    result.type = passage_type_t::NORMAL;
                }
                else if (field.value == "END")
                {
                    result.type = passage_type_t::END;
                }
                else 
                {
                    std::cerr << "[WARNING]: Invalid passage type. Falling back to 'normal'.\n";
                    result.type = passage_type_t::NORMAL;
                }
            }
            else if (field.name == "text")
            {
                result.text = field.value;
            }
            else if (field.name == "options")
            {
                result.options = process_options(field.value);
            }
        }
        
        return result;
    }
}

int main()
{
    return 0;
}