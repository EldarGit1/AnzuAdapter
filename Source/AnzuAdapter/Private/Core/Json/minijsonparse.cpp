#include "minijsonparse.h"

namespace AnzuMiniJson
{
    void JSONParser::skipWhitespace( std::istringstream& stream ) const
    {
        while ( stream.peek() == ' ' || stream.peek() == '\n' || stream.peek() == '\r' || stream.peek() == '\t' ) {
            stream.get();
        }
    }

    std::shared_ptr<JSONValue> JSONParser::parseValue( std::istringstream& stream ) const
    {
        skipWhitespace( stream );

        switch ( stream.peek() ) {
            case 'n':
                stream.ignore( 4 ); // Ignore "null"
                return std::make_shared<JSONValue>();
            case 't':
                stream.ignore( 4 ); // Ignore "true"
                return std::make_shared<JSONValue>( true );
            case 'f':
                stream.ignore( 5 ); // Ignore "false"
                return std::make_shared<JSONValue>( false );
            case '\"':
                return parseString( stream );
            case '{':
                return parseObject( stream );
            case '[':
                return parseArray( stream );
            default:
                return parseNumber( stream );
        }
    }

    std::shared_ptr<JSONValue> JSONParser::parseNumber( std::istringstream& stream ) const
    {
        double value;
        stream >> value;
        return std::make_shared<JSONValue>( value );
    }

    std::shared_ptr<JSONValue> JSONParser::parseString( std::istringstream& stream ) const
    {
        stream.ignore(); // Ignore the opening quote
        std::string value;
        char c;
        while ( stream.get( c ) ) {
            if ( c == '\\' ) {
                if ( stream.get( c ) ) {
                    if ( c == 'n' ) {
                        value += '\n';
                    } else if ( c == 't' ) {
                        value += '\t';
                    } else if ( c == 'r' ) {
                        value += '\r';
                    } else if ( c == 'b' ) {
                        value += '\b';
                    } else if ( c == 'f' ) {
                        value += '\f';
                    } else if ( c == '\\' || c == '\"' || c == '/' ) {
                        value += c;
                    } else {
                        throw std::runtime_error( "Invalid escape sequence in JSON string" );
                    }
                } else {
                    throw std::runtime_error( "Invalid JSON string syntax" );
                }
            } else if ( c == '\"' ) {
                break;
            } else {
                value += c;
            }
        }
        return std::make_shared<JSONValue>( value );
    }

    std::shared_ptr<JSONValue> JSONParser::parseObject( std::istringstream& stream ) const
    {
        stream.ignore(); // Ignore the opening brace
        JSONObject object;

        while ( stream.peek() != '}' ) {
            skipWhitespace( stream );
            std::string key = parseString( stream )->getString();
            skipWhitespace( stream );
            stream.ignore(); // Ignore the colon
            object[key] = parseValue( stream );
            skipWhitespace( stream );

            if ( stream.peek() == ',' ) {
                stream.ignore(); // Ignore the comma
            } else if ( stream.peek() != '}' ) {
                throw std::runtime_error( "Invalid JSON object syntax" );
            }
        }

        stream.ignore(); // Ignore the closing brace
        return std::make_shared<JSONValue>( object );
    }

    std::shared_ptr<JSONValue> JSONParser::parseArray( std::istringstream& stream ) const
    {
        stream.ignore(); // Ignore the opening bracket
        JSONArray array;

        while ( stream.peek() != ']' ) {
            array.push_back( parseValue( stream ) );
            skipWhitespace( stream );

            if ( stream.peek() == ',' ) {
                stream.ignore(); // Ignore the comma
            } else if ( stream.peek() != ']' ) {
                throw std::runtime_error( "Invalid JSON array syntax" );
            }
        }

        stream.ignore(); // Ignore the closing bracket
        return std::make_shared<JSONValue>( array );
    }

    std::shared_ptr<JSONValue> JSONParser::parse( const std::string& json_text ) const
    {
        std::istringstream stream( json_text );
        std::shared_ptr<JSONValue> result = parseValue( stream );
        skipWhitespace( stream );

        if ( stream.eof() ) {
            return result;
        } else {
            throw std::runtime_error( "Invalid JSON input: extra characters after the parsed value" );
        }
    }


    void printJSONValue( const std::shared_ptr<JSONValue>& value , int indent = 0 )
    {
        const std::string indentation( indent , ' ' );

        switch ( value->getType() ) {
            case JSONValue::Type::Null:
                std::cout << indentation << "null";
                break;
            case JSONValue::Type::Boolean:
                std::cout << indentation << ( value->getBoolean() ? "true" : "false" );
                break;
            case JSONValue::Type::Number:
                std::cout << indentation << value->getNumber();
                break;
            case JSONValue::Type::String:
                std::cout << indentation << "\"" << value->getString() << "\"";
                break;
            case JSONValue::Type::Object:
            {
                std::cout << indentation << "{\n";
                const JSONObject& object = value->getObject();
                for ( auto it = object.begin(); it != object.end(); ++it ) {
                    std::cout << indentation << "  \"" << it->first << "\": ";
                    printJSONValue( it->second , indent + 2 );
                    if ( std::next( it ) != object.end() ) {
                        std::cout << ",";
                    }
                    std::cout << "\n";
                }
                std::cout << indentation << "}";
                break;
            }
            case JSONValue::Type::Array:
            {
                std::cout << indentation << "[\n";
                const JSONArray& array = value->getArray();
                for ( size_t i = 0; i < array.size(); ++i ) {
                    printJSONValue( array[i] , indent + 2 );
                    if ( i < array.size() - 1 ) {
                        std::cout << ",";
                    }
                    std::cout << "\n";
                }
                std::cout << indentation << "]";
                break;
            }
        }
    }
} // end namespace AnzuMiniJson
