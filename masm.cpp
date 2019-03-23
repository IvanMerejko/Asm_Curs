//
// Created by ivan on 10.03.19.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include "assembler.h"
#include "masm.h"


masm::masm(const std::string &file)
    :asmFileName{file}{

};

void masm::createListingFile() {
    firstView();
    checkAllUsedButNotDeclaredIdentifiers();
    secondView();
}
void masm::secondView() {
    std::ifstream asmFile(asmFileName );

    if(!asmFile.is_open()){
        std::cout << "No file";
    }
    size_t currentLine{0};
    std::string oneStringFromAsmFile;

    while (std::getline(asmFile , oneStringFromAsmFile) && currentLine <= line){
        ++currentLine;
        std::cout << oneStringFromAsmFile << '\n' ;
        oneStringFromAsmFile.erase(std::remove_if(oneStringFromAsmFile.begin() ,
                                                  oneStringFromAsmFile.end()   ,
                                                  [](const char symbol){ return symbol == ' ' || symbol == '\t';}),
                                   oneStringFromAsmFile.end());
        if(!oneStringFromAsmFile.empty() && isErrorInLine[currentLine]){
            std::cout << "Error" << std::endl;
        }

    }
}
void masm::firstView() {
    std::ifstream asmFile(asmFileName);

    if(!asmFile.is_open()){
        std::cout << "No file";
    }


    std::string oneStringFromAsmFile;
    while (std::getline(asmFile , oneStringFromAsmFile) && !endOfFile){
        ++line;

        assembler::createVectorOfWordsFromString(oneStringFromAsmFile , wordsInString);
        if(wordsInString.empty()) continue;
        /*
         *              LEXEM PARSING FOR FIRST TASK
         * */
      /*  auto lexem =  assembler::lexemParsing(wordsInString);
        std::cout << oneStringFromAsmFile << std::endl;
        printLexems(lexem);

        assembler::syntAnaliser(std::cout , lexem);*/

        /*
         * if we have error with label method return true
         * */
        if( (isErrorInLine[line] = takeLabelsFromLine()) || wordsInString.empty()) {
            wordsInString.clear();
            continue;
        }

        if(assembler::isIdentifier(wordsInString.front())){
            workWithIdentifier();
        } else {
            workWithCommand();
        }
        /*commandType != AsmLanguage::AssemblerReservedWordType::NONE ? workWithCommand() : workWithIdentifier();
*/

        wordsInString.clear();

    }
    asmFile.close();
}
std::string masm::getStringByType(const std::string& word , const assembler::WordType type){
    switch (type){
        case assembler::WordType::REGISTER:
            return  "REGISTER" ;
        case assembler::WordType::DIRECTIVE:
            return  "DIRECTIVE" ;
        case assembler::WordType::COMMAND:
            return  "COMMAND" ;
        case assembler::WordType::IDENTIFIER:
            return  std::string{"IDENTIFIER   "} + (assembler::isDeclaredIdentifier(word , _data , _code) ? "DECLARED" : "UNDECLARED") ;
        case assembler::WordType::INSTRUCTION:
            return  "INSTRUCTION" ;
        case assembler::WordType::CONSTANT:
            return  "CONSTANT" ;
        case assembler::WordType::ONE_SYMBOLE_LEXEM:
            return  "ONE_SYMBOLE_LEXEM" ;
    }
}
void masm::printLexems(const assembler::lexem_type& lexems){
    size_t i = 0;
    for(const auto& [word , type] : lexems){
            std::cout << std::setw(4) << std::left << i++ << std::setw(8) << std::left << word << std::setw(6) << word.length() << std::setw(20) << getStringByType(word , type) << std::endl;

    }
}
void masm::workWithCommand() {
    /*
     * if data segment isn`t opened - open it
     * if one data segment is opened and tried open one more  - ERROR
     * */

   std::string tmp{wordsInString.front()};
    std::transform(tmp.begin() , tmp.end() , tmp.begin() , ::tolower);
    if(tmp == assembler::instructionsVector().front()){ // .data
        if((isErrorInLine[line] =  _data.isOpen() )) {
            return;
        };
        _data.open();
        lastOpenedSegment = "data";
        return;
    }
    /*
     * if code segment isn`t opened - open it
     * if one code segment is opened and tried open one more  - ERROR
     **/
    if(tmp == assembler::instructionsVector().at(1)){ // .code
        if( (isErrorInLine[line] =  _code.isOpen() ) ) {
            return;
        }
        _code.open();
        lastOpenedSegment = "code";
        return;
    }

    if(tmp == assembler::instructionsVector().at(2)){ // .end
        endOfFile = true;
        return;
    }
    if((isErrorInLine[line] = !_code.isOpen()) && !assembler::isWordInVector({"model"} , wordsInString.front()) ){
        return;
    }



    std::string parameters{};
    for(size_t i = 1 ; i < wordsInString.size() ; ++i) parameters += wordsInString.at(i) + " ";
    isErrorInLine[line] = !assembler::getPointerForCommandByName(wordsInString.front() , parameters)->isCorrectOperands(line);

}
void masm::workWithIdentifier() {



    if((isErrorInLine[line] = ( !_data.isOpen() && !_code.isOpen())))
        return;

    if( (isErrorInLine[line] = (wordsInString.size() != 3 ) ) )
        return;

    auto identifierType = assembler::identifierType(wordsInString.at(1));

    if( (isErrorInLine[line] =  (identifierType == assembler::IdentifierType::INCORRECT_IDENTIFIER)) )
        return  ;

    typename assembler::identifier new_identifier (wordsInString.front() , identifierType , wordsInString.back());

    if( (isErrorInLine[line] =  !new_identifier.isCorrectIdentifierValue()))
        return ;

    /*
     * if we can`t push identifier - ERROR
     * */
    isErrorInLine[line] = pushIdentifier(std::move(new_identifier)) ;
}
bool masm::pushIdentifier(assembler::identifier&& new_identifier) {
    if(lastOpenedSegment == "data"){
        return _data.push(std::move(new_identifier));
    } else if ( lastOpenedSegment == "code") {
        return _code.push(std::move(new_identifier));
    }
    return false;
}
bool masm::takeLabelsFromLine() {
    //  ?
    if( !isLabel(wordsInString.front()) /* && (isErrorInLine[line] =  !_code.isOpen())*/ )
        return false;

    while (!wordsInString.empty()  && isLabel(wordsInString.front()) ){
        /*
         * _code.pushLabel returns true if label with this name already located in code or it has command`s name
         * */
        wordsInString.front().erase(wordsInString.front().end() - 1);
        if(wordsInString.front().empty()){
            return true;
        }
        if( _code.pushLabel(std::move(assembler::label(wordsInString.front().substr(0 ,wordsInString.front().size()) , 0))) ){
            return true;
        };
        for(size_t i = 0 ; i < wordsInString.size() - 1 ; ++i) {
            wordsInString.at(i) = wordsInString.at(i+1);
        }
        wordsInString.pop_back();
    }
    return false;
}
bool masm::isLabel(std::string_view  word) {
    return word.back() == ':';
}
void masm::checkAllUsedButNotDeclaredIdentifiers(){


    for(const auto& [name , lines] : assembler::userIdentifiers::getUsedLabels()){
        if( !_code.isLabelDeclared(name)){
            for(const auto line : lines){
                isErrorInLine[line] = true;
            }
        }
    }

    for(const auto& [name , lines] : assembler::userIdentifiers::getUsedIdentifiers()){
        if(!_data.isDeclaredIdentifier(name) && !_code.isDeclaredIdentifier(name)){
            for(const auto& line : lines){
                isErrorInLine[line] = true;
            }
        }
    }


}