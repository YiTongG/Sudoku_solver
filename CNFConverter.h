//
// Created by yitong on 2023/10/27.
//

#ifndef AILAB2_CNFCONVERTER_H
#define AILAB2_CNFCONVERTER_H

#include <vector>
#include <string>
using namespace std;
class CNFConverter {
private:
    enum TokenType {
        VAR,
        AND,
        OR,
        NOT,
        IMPLIES,
        BICONDITIONAL,
        OPEN_PAREN,
        CLOSE_PAREN,
    };

    struct Token {
        Token() {};
        TokenType type;
        string value;
        Token(TokenType t, string v);
    };

    vector<Token> tokens;
    int currentPosition;
    Token currentToken;
    vector<Token> tokenize(const string& expr);




public:
    CNFConverter();

    vector<string> convert(const vector<string>& exprs);
    struct Node {
        string type;
        vector<Node> children;

        Node() = default;
        Node(const string& type, const vector<Node>& children = {})
                : type(type), children(children) {}
    };



    Node parse(const vector<Token>& tokens, int& pos);
    vector<string> convertToCNF(Node& root);



    string tokenToString(const Token &token);

    void printTree(const Node &node, int level);

    void traverse(const Node &node);

    Node toCNF(const Node &node);

    string tokenTypeToString(TokenType type);

    int precedence(TokenType op);


    void printAST(const Node &node, int indent);


    string extractLiteral(const Node &node);

    void printCNF(const Node &node, bool newClause);

    Node simplifyCNF(const Node &node);

    void printTree(const Node &node, string prefix);


    Node factor(const vector<Token> &tokens, int &pos);

    Node expression(const vector<Token> &tokens, int &pos);

    Node and_expression(const vector<Token> &tokens, int &pos);

    Node implies_expression(const vector<Token> &tokens, int &pos);

    void simplifyDoubleNegation(Node &root);

    Node toCNFTree(Node root);

    string nodeToString(const Node &root);

    vector<string> convertBnf(const vector<string> &exprs);
};


#endif //AILAB2_CNFCONVERTER_H
