//
// Created by ivan on 10.03.19.
//

#ifndef CURS_ASSEMBLER_H
#define CURS_ASSEMBLER_H
//
// Created by ivan on 10.03.19.
//

#include <string>
#include <vector>
#include <algorithm>
#include <string_view>
#include <set>
#include <memory>
#include <map>

namespace assembler{






    using stringsVector = std::vector<std::string>;
    enum class WordType{
        IDENTIFIER,
        COMMAND,
        CONSTANT,
        DIRECTIVE,
        INSTRUCTION,
        LABEL,
        ONE_SYMBOLE_LEXEM,
        REGISTER
    };


    enum class IdentifierType{
        DB = 1,
        DW = 2,
        DD = 4 ,
        INCORRECT_IDENTIFIER
    };
    enum class CommandName : int{
         MOV    = 1,
         IMUL   = 2,
         IDIV   = 3,
         OR     = 4,
         CMP    = 5,
         JNG    = 6,
         AND    = 7,
         ADD    = 8,
        CWDE    = 9,
        MODEL   =10,
    };


    struct identifier {
    public:
        friend bool operator<(const identifier &lft, const identifier &rht);

    public:
        IdentifierType type;
        std::string name;
        std::string value;

        identifier(std::string name, IdentifierType type, std::string value)
                : name{std::move(name)}, type{type}, value{std::move(value)} {};

        identifier(std::string name, IdentifierType type)
                : name{std::move(name)}, type{type}, value{""} {};

        void setValue(const std::string &value);

        bool isCorrectIdentifierValue() const;

        void print() const;
    };
    struct label{
        friend bool operator <(const label& lft , const label& rht);
        std::string name;
        size_t position;
        label(std::string name , size_t pos)
                :name{std::move(name)} , position{pos}{};
    };
    struct segment{

    protected:
        std::set<identifier> identifiers;
        bool isCreate = false;
    public:
        virtual bool isOpen() const;
        virtual void open();
        virtual bool push(identifier&& identifier1);
        virtual void printIdentifiers();
        virtual bool isDeclaredIdentifier(const std::string& identifier)const;
    };
    struct data : public segment{
    };
    struct code : public segment{
    private:
        std::set<label> labels;
    public:
        code() = default;
        bool pushLabel(label&& label) ;
        bool isLabelDeclared(const std::string& label_name) const;
    };

    bool isDeclaredIdentifier(const std::string& identifier , const data& _data , const code& _code);

    const  stringsVector commandsVector();
    const  stringsVector directivesVector();
    const  stringsVector instructionsVector();
    const  stringsVector registers32Vector();
    const  stringsVector registers16Vector();
    const  stringsVector registers8Vector();
    const  stringsVector segmentRegisters();

    bool isWordInVector(const stringsVector &vector, std::string_view word);
    bool isCommand(std::string_view word);
    bool isDirective(std::string_view word);
    bool isInstruction(std::string_view word);
    bool isIdentifier(std::string_view word);
    bool isRegister(const std::string& string);
    bool isReservedWord(std::string_view word);


    void createVectorOfWordsFromString(const std::string&  string , stringsVector& wordsInString)  ;





    size_t directivePosition(std::string_view directive);
    IdentifierType identifierType(std::string_view identifier_type);





    using lexem_type = std::vector<std::pair<std::string , WordType>>;

    void removeSpacesAndTabs(std::string& string);
    WordType getTypeOfOperand(const std::string&  operand );

    void syntAnaliser(std::ostream& os , const lexem_type& vectorLexems);
    lexem_type  lexemParsing(const stringsVector& vectorOfOperands );




    void splitByDelimiters(const std::string& delimiters  , stringsVector & vector);
    bool isCorrectAddressExpression(const stringsVector& operands , size_t line);
    bool isCorrectExpressionBetweenParentheses(const stringsVector& expressionOperands);
    class Command{
    protected:
        stringsVector operands;
    public:
        Command() = default;
        explicit Command(const std::string& string){
            std::string help{string};
            removeSpacesAndTabs(help);
            createVectorOfWordsFromString(help , operands);
            splitByDelimiters("{}," , operands);
        };
        virtual ~Command() = default;
        virtual bool isCorrectOperands(size_t line) = 0;

    };
    class Mov : public Command{
    public:
        Mov() = default;
        explicit Mov(const std::string& string)
            :Command(string){};
        ~Mov() override = default ;
        bool isCorrectOperands(size_t line) override ;
        bool isCorrectFirstOperand();
        bool isCorrectSecondOperand();
    };
    class Imul : public Command{
    public:
        Imul() = default;
        explicit Imul(const std::string& string)
                :Command(string){};
        ~Imul() override = default ;
        bool isCorrectOperands(size_t line) override;
    };
    class Idiv : public Command{
    public:
        Idiv() = default;
        explicit Idiv(const std::string& string)
                :Command(string){
            splitByDelimiters("[]*" , operands);
        };
        ~Idiv() override = default ;
        bool isCorrectOperands(size_t line) override;
    };
    class Or : public Command{
    public:
        Or() = default;
        explicit Or(const std::string& string)
                :Command(string){};
        ~Or() override = default ;
        bool isCorrectOperands(size_t line) override;
        bool isCorrectFirstOperand();
        bool isCorrectSecondOperand();
    };
    class Cmp : public Command{
    public:
        Cmp() = default;
        explicit Cmp(const std::string& string)
                :Command(string){};
        ~Cmp() override = default ;
        bool isCorrectOperands(size_t line) override;
        bool isCorrectFirstOperand();
        bool isCorrectSecondOperand(size_t line);
    };
    class Jng : public Command{
    public:
        Jng() = default;
        explicit Jng(const std::string& string)
                :Command(string){};
        ~Jng() override = default ;
        bool isCorrectOperands(size_t line) override ;
    };
    class And : public Command{
    public:
        And() = default;
        explicit And(const std::string& string)
                :Command(string){};
        ~And() override = default ;
        bool isCorrectOperands(size_t line) override;
        bool isCorrectFirstOperand(size_t line);
        bool isCorrectSecondOperand();
    };
    class Add : public Command{
    public:
        Add() = default;
        explicit Add(const std::string& string)
                :Command(string){};
        ~Add() override = default ;
        bool isCorrectOperands(size_t line) override;
        bool isCorrectFirstOperand(size_t line);
        bool isCorrectSecondOperand();
    };
    class Cwde : public Command{
    public:
        Cwde() = default;
        explicit Cwde(const std::string& string)
                :Command(string){};
        ~Cwde() override = default ;
        bool isCorrectOperands(size_t line) override ;
    };
    class Model : public Command{
    public:
        Model() = default;
        explicit Model(const std::string& string)
                :Command(string){};
        ~Model() override = default ;
        bool isCorrectOperands(size_t line) override ;
    };


    std::unique_ptr<Command> getPointerForCommandByName(std::string_view command , const std::string& operands);

    namespace userIdentifiers{
        using  mapOfIdentifiers = std::map<std::string , std::vector<size_t>>;
        mapOfIdentifiers& getLabels();
        mapOfIdentifiers& getIdentifiers();


        static void pushLabel(const std::string& name , size_t line);
        static void pushIdentifier(const std::string& name , size_t line);

    }

}
#endif //CURS_ASSEMBLER_H
