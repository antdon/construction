#include <string_view>
#include <variant>
#include <string>
#include <functional>
#include <tuple>

template <typename T>
class Success {
    T data;
    std::string_view remaining;
};

class Failure {
    int lineNumber;
    std::string expected;
    char found;
};

class Parser {

private:
    // parsers
    std::variant<Success<void>, Failure> collon(std::string_view input);
    std::variant<Success<void>, Failure> space(std::string_view input);
    std::variant<Success<char>, Failure> character(std::string_view input);

    // helper
    template <typename T> struct ParserFunc {}; // std::function<std::variant<Success<T>, Failure>(std::string_view)>
    template <typename T> struct ExtractType {};
    template <typename T> struct ExtractType<ParserFunc<T>> {
	using type = T;
    };
   

    // combinator
    template <typename T>
    std::variant<Success<std::vector<T>>, Failure> repeatTillFailure(std::function<std::variant<Success<T>, Failure>(std::string_view)> parser);

    template <typename... Parsers>
    auto sequence(Parsers... parsers) -> std::tuple<typename ExtractType<Parsers...>::type>;


    // grammar components
    
};


