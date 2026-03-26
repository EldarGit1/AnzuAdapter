#ifndef _MINI_JSON_PARSE_H_
#define _MINI_JSON_PARSE_H_

#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace AnzuMiniJson
{
    class JSONValue;

    typedef std::map<std::string , std::shared_ptr<JSONValue>> JSONObject;
    typedef std::vector<std::shared_ptr<JSONValue>> JSONArray;

    class JSONValue
    {
    public:
        enum class Type
        {
            Null ,
            Boolean ,
            Number ,
            String ,
            Object ,
            Array
        };

        JSONValue() : type( Type::Null ) {}
        JSONValue( bool value ) : type( Type::Boolean ) , boolean_value( value ) {}
        JSONValue( double value ) : type( Type::Number ) , number_value( value ) {}
        JSONValue( const std::string& value ) : type( Type::String ) , string_value( value ) {}
        JSONValue( const JSONObject& value ) : type( Type::Object ) , object_value( value ) {}
        JSONValue( const JSONArray& value ) : type( Type::Array ) , array_value( value ) {}

        Type getType() const { return type; }

        bool getBoolean() const { return boolean_value; }
        double getNumber() const { return number_value; }
        const std::string& getString() const { return string_value; }
        const JSONObject& getObject() const { return object_value; }
        const JSONArray& getArray() const { return array_value; }

    private:
        Type type;
        bool boolean_value;
        double number_value;
        std::string string_value;
        JSONObject object_value;
        JSONArray array_value;
    };


    class JSONParser
    {
    public:
        JSONParser() = default;
        std::shared_ptr<JSONValue> parse( const std::string& json_text ) const;

    private:
        std::shared_ptr<JSONValue> parseValue( std::istringstream& stream ) const;
        std::shared_ptr<JSONValue> parseNumber( std::istringstream& stream ) const;
        std::shared_ptr<JSONValue> parseString( std::istringstream& stream ) const;
        std::shared_ptr<JSONValue> parseObject( std::istringstream& stream ) const;
        std::shared_ptr<JSONValue> parseArray( std::istringstream& stream ) const;
        void skipWhitespace( std::istringstream& stream ) const;
    };
}

#endif // _MINI_JSON_PARSE_H_
