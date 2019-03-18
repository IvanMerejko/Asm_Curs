//
// Created by ivan on 10.03.19.
//

#include <string>
#include <vector>
#include <algorithm>
#include <string_view>
#include <set>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "assembler.h"
namespace assembler{

    void identifier::setValue(const std::string &value) {
        this->value = value;
    };
    bool identifier::isCorrectIdentifierValue() const {
            auto positionOfMinusInValue = std::find(value.begin(), value.end(), '-');
            auto tmp = positionOfMinusInValue == value.begin() ? value.substr(1, value.size()) : value;
            auto lastLetter = static_cast <char> ( std::tolower(static_cast<unsigned char> (value.back())));
            switch (lastLetter) {
                case 'h':
                    for (const auto &it : tmp.substr(0, tmp.size() - 1)) {
                        if (auto one_value = static_cast<int>(it);  one_value < 48 ||
                                                                    (one_value > 57 && one_value < 65) ||
                                                                    (one_value > 70 && one_value < 97) ||
                                                                    one_value > 102) {
                            return false;
                        }
                    }
                    break;
                case 'b':
                    for (const auto &it : tmp.substr(0, tmp.size() - 2)) {
                        if (auto one_value = static_cast<int>(it); one_value < 48 || one_value > 49) {
                            return false;
                        }
                    }
                    break;
                default:
                    if (tmp.empty()) return false;
                    for (const auto &it : tmp) {
                        if (auto one_value = static_cast<int>(it);  one_value < 48 || one_value > 57) {
                            return false;
                        }
                    }
                    break;
            }
            return true;
        };
    void identifier::print() const {}

    bool segment::isOpen() const {
            return isCreate;
    };
    void segment::open() {
        isCreate = true;
    }
    bool segment::push(assembler::identifier &&identifier1) {
        auto it = identifiers.insert(std::move(identifier1));
        return !it.second;
    }
    bool segment::isDeclaredIdentifier(const std::string &new_identifier) const {
        return std::find_if(identifiers.cbegin() , identifiers.cend() , [&new_identifier](const identifier& currentIdentifier){
            return currentIdentifier.name == new_identifier;
        }) != identifiers.cend();
    }
    void segment::printIdentifiers() {
        for(const auto &it : identifiers){
            it.print();
        }
    }
    /*                  TODO
     *
     *
     * */
    bool code::isLabelDeclared(const std::string &identifier) const {

    }
    bool code::pushLabel(assembler::label &&label) {
        auto it = labels.insert(std::move(label));
        return !it.second;
    }

    bool operator <(const identifier& lft , const identifier& rht){
        return lft.name < rht.name;
    }
    bool operator <(const label& lft , const label& rht){
        return lft.name < rht.name;
    }


    const  stringsVector commandsVector() {
        static const  stringsVector commands{
                "mov",
                "imul",
                "idiv",
                "or",
                "cmp",
                "jng",
                "and",
                "add",
                "cwde"

        };
        return commands;
    }
    const  stringsVector directivesVector() {
        static const  stringsVector directives{
                "db",
                "dw",
                "dd"
        };
        return directives;
    }
    const  stringsVector instructionsVector(){
        static const stringsVector instructions{
                ".data",
                ".code",
                "end",
                "model"
        };
        return instructions;
    }
    const  stringsVector registers32Vector() {
        static const  stringsVector registers32{
                "eax",
                "ebx",
                "edx",
                "ecx",
                "esi",
                "edi"
        };
        return registers32;
    }
    const  stringsVector registers16Vector() {
        static const  stringsVector registers16{
                "ax",
                "bx",
                "cx",
                "dx",
                "si",
                "di"
        };
        return registers16;
    }
    const  stringsVector registers8Vector() {
        static const  stringsVector registers8{
                "ah",
                "al",
                "bh",
                "bl",
                "ch",
                "cl"
        };
        return registers8;
    }
    const  stringsVector segmentRegisters(){
        static const  stringsVector segmentsRegisters{
                "cs",
                "ds",
                "es",
                "ss",
        };
        return segmentsRegisters;
    }

    bool isWordInVector(const stringsVector &vector, std::string_view word) {
        std::string tmp{word};
        tmp.erase(std::remove_if(tmp.begin() , tmp.end() , [](char s){
            return s == ' ' || s == '\t';
        }) , tmp.end());
        for(const auto &it : vector){
            std::transform(tmp.begin() , tmp.end() ,tmp.begin(), ::tolower );
            if(tmp == it) {
                return true;
            }
        }
        return false;
    }
    bool isCommand(std::string_view word) {
        return isWordInVector(commandsVector() , word);
    }
    bool isDirective(std::string_view word) {
        return isWordInVector(directivesVector() , word);
    }
    bool isInstruction(std::string_view word) {
        return isWordInVector(instructionsVector() , word);
    }
    bool isReservedWord(std::string_view word) {
        return isWordInVector(registers16Vector() , word)       ||
               isWordInVector(registers8Vector() , word)        ||
               isWordInVector(registers32Vector() , word)       ||
               isWordInVector(segmentRegisters() , word)       ||
               isCommand(word) || isDirective(word) || isInstruction(word) ;
    }



    void createVectorOfWordsFromString(const std::string& string , stringsVector& wordsInString) {
        std::istringstream inputStringStream(string);
        std::string tmp;
        while (inputStringStream >> tmp){
            wordsInString.push_back(tmp);

        }
    }

    bool isIdentifier(std::string_view word) {
        return !(isCommand(word) || isDirective(word) || isInstruction(word));

    }

    size_t directivePosition(std::string_view directive) {
        auto size = directivesVector().size();
        for(size_t i = 0 ; i < size; ++i){
            if( directive == directivesVector().at(i) ){
                return i;
            }
        }
        return directivesVector().size();
    }
    IdentifierType identifierType(std::string_view identifier_type) {
        auto directive_pos = directivePosition(identifier_type);
        switch(directive_pos){
            case 0:
                return IdentifierType::DB;
            case 1:
                return IdentifierType::DW;
            case 2:
                return IdentifierType::DD;
            default:
                return IdentifierType::INCORRECT_IDENTIFIER;
        }

    }





    bool isDeclaredIdentifier(const std::string& identifier , const data& _data , const code& _code){
        return _data.isDeclaredIdentifier(identifier) ||  _code.isDeclaredIdentifier(identifier) || _code.isLabelDeclared(identifier);
    }


    bool Mov::isCorrectOperands() {};
    bool Imul::isCorrectOperands() {
        return operands.size() == 1 && (isWordInVector(registers8Vector() , operands.front()) ||
                                        isWordInVector(registers16Vector() , operands.front()) ||
                                        isWordInVector(registers32Vector() , operands.front()) );
    }; //done 
    bool Idiv::isCorrectOperands() {};
    bool Or::isCorrectOperands() {
            if(operands.size() != 3){
                return false;
            }
        return isCorrectFirstOperand() && isCorrectSecondOperand();
    };
    bool Or::isCorrectFirstOperand() {
        return isWordInVector(registers8Vector() , operands.front()) ||
               isWordInVector(registers16Vector() , operands.front())||
               isWordInVector(registers32Vector() , operands.front());
    }
    bool Or::isCorrectSecondOperand() {
        return isWordInVector(registers8Vector() , operands.back()) ||
               isWordInVector(registers16Vector() , operands.back())||
               isWordInVector(registers32Vector() , operands.back());
    }
    bool Cmp::isCorrectOperands() {};
    bool Jng::isCorrectOperands() {};
    bool And::isCorrectOperands() {};
    bool Add::isCorrectOperands() {};
    bool Cwde::isCorrectOperands() {};

    void removeSpacesAndTabs(std::string& string){
        string.erase(std::remove_if(string.begin() , string.end() , [](char s){
            return s == ' ' || s == '\t';
        }) , string.end()) ;
    }



    void syntAnaliser(std::ostream& os , const lexem_type& vectorLexems){

        auto commaPosition = std::find_if(vectorLexems.cbegin() , vectorLexems.cend() , [](const std::pair<std::string , WordType>& lexem){
            return lexem.first == ",";
        });

        auto commandPos = std::find_if(vectorLexems.cbegin() , vectorLexems.cend() , [](const std::pair<std::string , WordType>& lexem){
            return lexem.second == WordType::COMMAND;
        });
        auto first = vectorLexems.front();

        if(first.second == WordType::IDENTIFIER){
            if(commandPos == vectorLexems.cend()){
                    auto directive = std::find_if(vectorLexems.cbegin() , vectorLexems.cend() , [](const std::pair<std::string , WordType>& lexem){
                        return lexem.second == WordType::DIRECTIVE;
                    });
                if(directive == vectorLexems.end()){
                    os    << "mitkaIndex = 0, "
                          << "mnemokodIndex = -1"
                          << ", mnemokodCount =  -1"
                          << ", firstOperandIndex = -1"
                          << ", firstOperandCount = -1"
                          << ", secondOperandIndex = -1 , "
                           << "secondOperandCount = -1" << std::endl;
                } else {
                    os    << "mitkaIndex = 0, "
                          << "mnemokodIndex = 1"
                          << ", mnemokodCount =  1"
                          << ", firstOperandIndex = 2"
                          << ", firstOperandCount = 1"
                          << ", secondOperandIndex = -1 , "
                          << "secondOperandCount = -1" << std::endl;
                }
            } else {
                os    << "mitkaIndex = 0, "
                      << "mnemokodIndex = " << commandPos - vectorLexems.begin()
                      << ", mnemokodCount =  1"
                      << ", firstOperandIndex = " << commandPos - vectorLexems.begin() + 1
                      << ", firstOperandCount = " << commaPosition - commandPos - 1;
                if(commaPosition == vectorLexems.cend()){
                    os << ", secondOperandIndex = -1 , "
                       << "secondOperandCount = -1" << std::endl;
                } else {
                    os << ", secondOperandIndex = " << commaPosition - vectorLexems.begin() + 1
                       << " , secondOperandCount = "<< vectorLexems.cend() - commaPosition - 1 << std::endl;
                }
            }
        } else if (first.second == WordType::COMMAND){
            os << "mitkaIndex     = -1,  "
               << "mnemokodIndex  = 0, "
               << "mnemokodCount  = 1 , ";
                  if(vectorLexems.size() == 1){
                      os  << "firstOperandIndex = -1 , "
                          << "firstOperandCount = -1" ;
                  } else {
                      os  << "firstOperandIndex = 1 , "
                          << "firstOperandCount = " << commaPosition - commandPos - 1;
                  }


            if(commaPosition == vectorLexems.cend()){
                os << ", secondOperandIndex = -1 , "
                   << "secondOperandCount = -1" << std::endl;
            } else {
                os << ", secondOperandIndex = " << commaPosition - commandPos + 1
                   << " , secondOperandCount = "<< vectorLexems.cend() - commaPosition - 1 << std::endl;
            }
        } else {
            os    << "mitkaIndex = -1,  "
                  << "mnemokodIndex = 0"
                  << ", mnemokodCount =  1"
                  << ", firstOperandIndex = -1"
                  << ", firstOperandCount = -1"
                  << ", secondOperandIndex = -1 , "
                  << "secondOperandCount = -1" << std::endl;
        }




    }


    WordType getTypeOfOperand(const std::string& operand){
        if(isWordInVector(commandsVector() , operand)){
            return WordType::COMMAND;
        } else if(isWordInVector(registers8Vector() , operand)  ||
                  isWordInVector(registers16Vector() , operand) ||
                  isWordInVector(registers32Vector() , operand) ||
                  isWordInVector(segmentRegisters() , operand)  ){
            return WordType::REGISTER;
        } else if(isWordInVector(directivesVector() , operand)) {
            return WordType::DIRECTIVE;
        } else if (identifier{"" , IdentifierType::INCORRECT_IDENTIFIER , operand}.isCorrectIdentifierValue()){
            return WordType::CONSTANT;
        } else {
            return WordType::IDENTIFIER;
        }
    }
    lexem_type  lexemParsing(const stringsVector& vectorOfOperands){

        if(vectorOfOperands.size() == 1  && isWordInVector(instructionsVector() , vectorOfOperands.front())){
            return {{vectorOfOperands.front() , WordType::INSTRUCTION}};
        }
        lexem_type lexems;
        std::string one_lexem;
        std::string delimiters = ":,[]*-";
        for(const auto& word : vectorOfOperands){
            auto current_delimiter_position = std::find_first_of(word.cbegin() , word.cend() ,
                                                                 delimiters.cbegin() , delimiters.cend());
            decltype(current_delimiter_position) previous_delimiter_position = word.cbegin();
            /*
             * check if delimiter is the first symbol in line
             * */
            while(current_delimiter_position == previous_delimiter_position){
                lexems.push_back({std::string{*current_delimiter_position} , WordType::ONE_SYMBOLE_LEXEM});
                if( ++current_delimiter_position == word.cend()){
                    break;
                } else {
                    current_delimiter_position = std::find_first_of(current_delimiter_position  , word.cend() ,
                                                                    delimiters.cbegin() , delimiters.cend());
                    ++previous_delimiter_position;
                }

            }
            while ( current_delimiter_position != word.cend()  ){
                std::string current_operand {previous_delimiter_position , current_delimiter_position};
                removeSpacesAndTabs(current_operand);
                lexems.push_back(
                        {current_operand , getTypeOfOperand(current_operand)}
                        );
                lexems.push_back(
                        {std::string{*current_delimiter_position} , WordType::ONE_SYMBOLE_LEXEM}
                        );
                previous_delimiter_position = current_delimiter_position + 1;
                current_delimiter_position = std::find_first_of(current_delimiter_position + 1 , word.cend() ,
                                                                delimiters.cbegin() , delimiters.cend());
            }
            if(previous_delimiter_position != word.cend() && word.size() != 1 ){
                lexems.push_back(
                        {std::string{previous_delimiter_position , word.cend()} ,
                         getTypeOfOperand(std::string(previous_delimiter_position , word.cend()))}
                         );
            }
            std::string oneValue = "1234567890";
            if(std::find(oneValue.begin() , oneValue.end() , word.front()) != oneValue.end() && word.size() == 1 ){
                lexems.push_back(
                        {std::string{previous_delimiter_position , word.cend()} ,
                         WordType ::CONSTANT}
                );
            }

        }

        return lexems;
    };



    void splitByDelimiters(const std::string& delimiters  , stringsVector& wordsInString){
        stringsVector newVector;
        for(const auto& word : wordsInString){
            auto current_delimiter_position = std::find_first_of(word.cbegin() , word.cend() ,
                                                                 delimiters.cbegin() , delimiters.cend());
            decltype(current_delimiter_position) previous_delimiter_position = word.cbegin();
            /*
             * check if delimiter is the first symbol in line
             * */
            while(current_delimiter_position == previous_delimiter_position){
                newVector.push_back(std::string{*current_delimiter_position} );

                if( ++current_delimiter_position == word.cend()){
                    break;
                } else {
                    current_delimiter_position = std::find_first_of(current_delimiter_position  , word.cend() ,
                                                                    delimiters.cbegin() , delimiters.cend());
                    ++previous_delimiter_position;
                }

            }
            while ( current_delimiter_position != word.cend()  ){
                std::string current_operand {previous_delimiter_position , current_delimiter_position};
                removeSpacesAndTabs(current_operand);
                newVector.push_back(current_operand);
                newVector.push_back(std::string{*current_delimiter_position});
                previous_delimiter_position = current_delimiter_position + 1;
                current_delimiter_position = std::find_first_of(current_delimiter_position + 1 , word.cend() ,
                                                                delimiters.cbegin() , delimiters.cend());
            }
            if(previous_delimiter_position != word.cend() && word.size() != 1 ){
                newVector.push_back(std::string{previous_delimiter_position , word.cend()});
            }
            std::string oneValue = "1234567890";
            if(std::find(oneValue.begin() , oneValue.end() , word.front()) != oneValue.end() && word.size() == 1 ){
                newVector.push_back(std::string{previous_delimiter_position , word.cend()});
            }
        }
        wordsInString = std::move(newVector);
    }
    std::unique_ptr<Command> getPointerForCommandByName(std::string_view command , const std::string& operands){
        size_t numberOfCommand = 0;
        auto commands = commandsVector();
        for(size_t i = 0 ; i < commands.size() ; ++i){
            if(commands[i] == command){
                numberOfCommand = i;
                break;
            };
        }
        switch(static_cast<CommandName >(numberOfCommand + 1)){
            case CommandName::MOV:
                return std::make_unique<Mov>(operands);
            case CommandName::IMUL:
                return std::make_unique<Imul>(operands);
            case CommandName::IDIV:
                return std::make_unique<Idiv>(operands);
            case CommandName::OR:
                return std::make_unique<Or>(operands);
            case CommandName::CMP:
                return std::make_unique<Cmp>(operands);
            case CommandName::JNG:
                return std::make_unique<Jng>(operands);
            case CommandName::AND:
                return std::make_unique<And>(operands);
            case CommandName::ADD:
                return std::make_unique<Add>(operands);
            case CommandName::CWDE:
                return std::make_unique<Cwde>(operands);
        }
    }
}