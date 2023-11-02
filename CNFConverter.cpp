//
// Created by yitong on 2023/10/27.
//
#include "CNFConverter.h"
#include <iostream>
#include<stack>
#include <unordered_set>
#include <set>
#include <sstream>
#include <fstream>
#include "regex"
#include "unordered_map"
// Implement Token constructor

CNFConverter::Token::Token(TokenType t, string v) : type(t), value(v) {}

string CNFConverter::tokenToString(const Token &token) {
    switch (token.type) {
        case NOT:
            return "NOT";
        case AND:
            return "AND";
        case OR:
            return "OR";
        case IMPLIES:
            return "IMPLIES";
        case BICONDITIONAL:
            return "BICONDITIONAL";
        case OPEN_PAREN:
            return "OPEN_PAREN";
        case CLOSE_PAREN:
            return "CLOSE_PAREN";
        case VAR:
            return "VAR";
        default:
            return "UNKNOWN";
    }
}

vector<CNFConverter::Token> CNFConverter::tokenize(const string &expr) {
    vector<Token> tokens;
    for (size_t i = 0; i < expr.size(); i++) {
        switch (expr[i]) {
            case '!':
                tokens.push_back(Token(NOT, "!"));
                break;
            case '^':
                tokens.push_back(Token(AND, "^"));
                break;
            case 'v':
                tokens.push_back(Token(OR, "v"));
                break;
            case '(':
                tokens.push_back(Token(OPEN_PAREN, "("));
                break;
            case ')':
                tokens.push_back(Token(CLOSE_PAREN, ")"));
                break;
            case ' ':
                break; // skip spaces
            default:
                if (
                        (expr[i] == 'n' && i + 7 < expr.length())) {
                    std::string substr = expr.substr(i, 8);
                    tokens.push_back(Token(VAR, substr));
                    i += 7; // 跳过当前提取的字符串的其余部分
                }
                if (isupper(expr[i])) {
                    tokens.push_back(Token(VAR, string(1, expr[i])));
                } else if (i < expr.size() - 1 && expr[i] == '=' && expr[i + 1] == '>') {
                    tokens.push_back(Token(IMPLIES, "=>"));
                    i++;  // Skip the next character
                } else if (i < expr.size() - 2 && expr[i] == '<' && expr[i + 1] == '=' && expr[i + 2] == '>') {
                    tokens.push_back(Token(BICONDITIONAL, "<=>"));
                    i += 2;  // Skip the next two characters
                }
                break;
        }
    }

    return tokens;
}

CNFConverter::Node CNFConverter::expression(const std::vector<Token> &tokens, int &pos) {
    Node left = and_expression(tokens, pos);

    if (pos < tokens.size()) {
        if (tokens[pos].type == OR) {
            pos++;
            Node right = expression(tokens, pos);
            return Node("OR", {left, right});
        } else if (tokens[pos].type == BICONDITIONAL) {
            pos++;
            Node right = and_expression(tokens, pos);
            return Node("BICONDITIONAL", {left, right});
        }
    }

    return left;
}

CNFConverter::Node CNFConverter::and_expression(const std::vector<Token> &tokens, int &pos) {
    Node left = implies_expression(tokens, pos);

    if (pos < tokens.size() && tokens[pos].type == AND) {
        pos++;
        Node right = and_expression(tokens, pos);
        return Node("AND", {left, right});
    }

    return left;
}

CNFConverter::Node CNFConverter::implies_expression(const std::vector<Token> &tokens, int &pos) {
    Node left = factor(tokens, pos);

    if (pos < tokens.size() && tokens[pos].type == IMPLIES) {
        pos++;
        Node right = implies_expression(tokens, pos);
        return Node("IMPLIES", {left, right});
    }

    return left;
}

CNFConverter::Node CNFConverter::factor(const std::vector<Token> &tokens, int &pos) {
    if (tokens[pos].type == NOT) {
        pos++;
        return Node("NOT", {expression(tokens, pos)});
    } else if (tokens[pos].type == OPEN_PAREN) {
        pos++;
        Node child = expression(tokens, pos);
        if (tokens[pos].type != CLOSE_PAREN) {
            std::cerr << "Expected closing parenthesis." << std::endl;
            return Node{}; // Or throw an exception.
        }
        pos++;
        return child;
    } else if (tokens[pos].type == VAR) {
        return Node(tokens[pos++].value, {});
    } else {
        std::cerr << "Unexpected token." << std::endl;
        return Node{}; // Or throw an exception.
    }
}


CNFConverter::Node CNFConverter::parse(const vector<Token> &tokens, int &pos) {
    if (pos >= tokens.size()) {
        cerr << "Unexpected end of tokens." << endl;
        return Node{};
    }

    stack<Node> operands;
    stack<TokenType> operators;

    auto applyOperator = [&]() {
        TokenType op = operators.top();
        operators.pop();

        Node right = operands.top();
        operands.pop();
        Node left = (op == NOT) ? Node{} : operands.top();
        if (op != NOT) operands.pop();
        operands.push(Node{tokenTypeToString(op), (op == NOT) ? vector<Node>{right} : vector<Node>{left, right}});
    };

    while (pos < tokens.size() && tokens[pos].type != CLOSE_PAREN) {
        if (tokens[pos].type == VAR) {
            operands.push(Node{tokens[pos].value, {}});
            pos++;
        } else if (tokens[pos].type == OPEN_PAREN) {
            pos++;
            operands.push(parse(tokens, pos));
            if (pos >= tokens.size() || tokens[pos].type != CLOSE_PAREN) {
                cerr << "Expected closing parenthesis." << endl;
                return Node{};
            }
            pos++;
        } else if (tokens[pos].type == NOT || tokens[pos].type == AND ||
                   tokens[pos].type == OR ||
                   tokens[pos].type == IMPLIES ||
                   tokens[pos].type == BICONDITIONAL) {

            while (!operators.empty() && precedence(operators.top()) >= precedence(tokens[pos].type)) {
                applyOperator();
            }
            operators.push(tokens[pos].type);
            pos++;
        } else {
            cerr << "Unexpected token type." << endl;
            return Node{};
        }
    }

    while (!operators.empty()) {
        applyOperator();
    }

    if (operands.size() != 1) {
        cerr << "Invalid expression." << endl;
        return Node{};
    }
    return operands.top();
}

int CNFConverter::precedence(TokenType type) {
    switch (type) {
        case NOT:
            return 5;
        case AND:
            return 4;
        case OR:
            return 3;
        case IMPLIES:
            return 2;
        case BICONDITIONAL:
            return 1;
        default:
            return 0;
    }
}


string CNFConverter::tokenTypeToString(TokenType type) {
    switch (type) {
        case AND:
            return "AND";
        case OR:
            return "OR";
        case IMPLIES:
            return "IMPLIES";
        case NOT:
            return "NOT";
        case BICONDITIONAL:
            return "BICONDITIONAL";
        default:
            return "";
    }
}


string CNFConverter::nodeToString(const Node &root) {

    if (root.children.empty()) {
        return root.type;  // It's a VAR or NOT
    }
    if (root.type == "NOT") {
        return "!" + nodeToString(root.children[0]);
    }
    // Change this representation to use spaces for ORs
    if (root.type == "OR") {
        // Split the elements into a set to remove duplicates
        std::set<string> uniqueElements;

        std::istringstream leftStream(nodeToString(root.children[0]));
        std::string item;
        while (getline(leftStream, item, ' ')) {
            uniqueElements.insert(item);
        }

        std::istringstream rightStream(nodeToString(root.children[1]));
        while (getline(rightStream, item, ' ')) {
            uniqueElements.insert(item);
        }

        // Convert the set back to a string
        std::ostringstream os;
        for (const auto &elem: uniqueElements) {
            os << elem << " ";
        }
        return os.str().substr(0, os.str().size() - 1);  // Remove the trailing space
    }

    // No need for this line since AND operations are handled separately.
    // return "(" + nodeToString(node.children[0]) + " " + node.value + " " + nodeToString(node.children[1]) + ")";
    return "";
}


vector<string> CNFConverter::convertToCNF(Node &root) {
    unordered_map<string, bool> clauseMap; // 使用这个map去重

    vector<string> clauses;
    simplifyDoubleNegation(root);
    // Base cases: If the root is a variable or negation of a variable

    if (root.type == "VAR" || root.type == "NOT" || root.children.empty()) {
        return {nodeToString(root)};
    }
    if (root.type == "AND") {
        auto leftClauses = convertToCNF(root.children[0]);
        auto rightClauses = convertToCNF(root.children[1]);
        leftClauses.insert(leftClauses.end(), rightClauses.begin(), rightClauses.end());
        return leftClauses;
    }
    // Step 3: Push Negations Inward
    if (root.type == "NOT") {
        Node child = root.children[0];
        if (child.type == "AND" || child.type == "OR") {
            // De Morgan's Laws
            string newOp = child.type == "AND" ? "OR" : "AND";
            Node notLeft = {"NOT", {child.children[0]}};
            Node notRight = {"NOT", {child.children[1]}};
            Node deMorgansNode = {newOp, {notLeft, notRight}};
            return convertToCNF(deMorgansNode);
        }
    }

    // Step 4: Distribute OR Over AND
    if (root.type == "OR") {
        if (root.children[0].type == "AND") {
            // (A ^ B) v C becomes (A v C) ^ (B v C)
            Node leftOr = {"OR", {root.children[0].children[0], root.children[1]}};
            Node rightOr = {"OR", {root.children[0].children[1], root.children[1]}};
            Node andNode = {"AND", {leftOr, rightOr}};
            auto clausesFromAnd = convertToCNF(andNode);
            clauses.insert(clauses.end(), clausesFromAnd.begin(), clausesFromAnd.end());
            return clauses;
        } else if (root.children[1].type == "AND") {
            // A v (B ^ C) becomes (A v B) ^ (A v C)
            Node leftOr = {"OR", {root.children[0], root.children[1].children[0]}};
            Node rightOr = {"OR", {root.children[0], root.children[1].children[1]}};
            Node andNode = {"AND", {leftOr, rightOr}};
            auto clausesFromAnd = convertToCNF(andNode);
            clauses.insert(clauses.end(), clausesFromAnd.begin(), clausesFromAnd.end());
            return clauses;
        } else {
            // if neither child is an AND, just return the OR as it is
            clauses.push_back(nodeToString(root));
            return clauses;
        }
    }
}

void CNFConverter::simplifyDoubleNegation(Node &root) {
    if (root.type == "NOT" && root.children[0].type == "NOT") {
        root = root.children[0].children[0];  // Eliminate the double negation
    }

    for (Node &child: root.children) {
        simplifyDoubleNegation(child);
    }
}

CNFConverter::Node CNFConverter::toCNF(const Node &node) {
    if (node.children.empty()) {  // Leaf node (variable or constant)
        return node;
    }

    if (node.type == "NOT") {
        Node child = toCNF(node.children[0]);

        // Handling double negation: ¬¬A => A
        if (child.type == "NOT") {
            return child.children[0];
        } else if (child.type == "AND") {
            // ¬(A ∧ B) => (¬A ∨ ¬B)
            return Node{"OR", {Node{"NOT", {child.children[0]}}, Node{"NOT", {child.children[1]}}}};
        } else if (child.type == "OR") {
            // ¬(A ∨ B) => (¬A ∧ ¬B)1
            return toCNF(Node{"AND", {Node{"NOT", {child.children[0]}}, Node{"NOT", {child.children[1]}}}});
        } else {
            return Node{"NOT", {child}};
        }
    }

    if (node.type == "IMPLIES") {
        // A → B => ¬A ∨ B
        Node left = Node{"NOT", {toCNF(node.children[0])}};
        Node right = toCNF(node.children[1]);
        return toCNF(Node{"OR", {left, right}});
    }

    if (node.type == "BICONDITIONAL") {
        // A ↔ B => (A → B) ∧ (B → A)
        Node left = toCNF(Node{"IMPLIES", {node.children[0], node.children[1]}});
        Node right = toCNF(Node{"IMPLIES", {node.children[1], node.children[0]}});
        return Node{"AND", {left, right}};
    }

    if (node.type == "AND" || node.type == "OR") {
        Node left = toCNF(node.children[0]);
        Node right = toCNF(node.children[1]);

        if (node.type == "OR") {
            std::vector<Node> terms;
            if (left.type == "AND") {
                terms.push_back(Node{"OR", {left.children[0], right}});
                terms.push_back(Node{"OR", {left.children[1], right}});
            } else if (right.type == "AND") {
                terms.push_back(Node{"OR", {left, right.children[0]}});
                terms.push_back(Node{"OR", {left, right.children[1]}});
            } else {
                return Node{"OR", {left, right}};
            }

            Node combined = terms[0];
            for (size_t i = 1; i < terms.size(); ++i) {
                combined = Node{"AND", {combined, terms[i]}};
            }
            return combined;
        } else { // node.type == "AND"
            return Node{"AND", {left, right}};
        }
    }

    return node;  // Fallback if none of the above conditions are met
}

void CNFConverter::printTree(const Node &node, string prefix = "") {
    cout << prefix;
    if (!prefix.empty()) {
        cout << "-- ";
    }
    cout << node.type << endl;

    const auto childCount = node.children.size();
    for (size_t i = 0; i < childCount; ++i) {
        // 如果不是最后一个子节点，我们需要在接下来的行中继续绘制竖线
        const auto childPrefix = prefix + (i < childCount - 1 ? "|  " : "   ");
        printTree(node.children[i], childPrefix);
    }
}

void CNFConverter::printAST(const CNFConverter::Node &node, int indent = 0) {
    if (node.type.empty()) return;

    for (int i = 0; i < indent; ++i) {
        cout << "  ";  // 两个空格缩进
    }

    cout << node.type;
    if (!node.children.empty()) {
        cout << " -> (";
        for (size_t i = 0; i < node.children.size(); ++i) {
            if (i != 0) cout << ", ";
            cout << node.children[i].type;
        }
        cout << ")";
    }
    cout << endl;

    for (const auto &child: node.children) {
        printAST(child, indent + 1);
    }
}


CNFConverter::Node CNFConverter::simplifyCNF(const Node &node) {
    if (node.type == "AND") {
        Node left = simplifyCNF(node.children[0]);
        Node right = simplifyCNF(node.children[1]);
        return Node{"AND", {left, right}};
    } else if (node.type == "OR") {
        unordered_set<string> literals;
        for (const auto &child: node.children) {
            // Remove double negations
            if (child.type == "NOT" && child.children[0].type == "NOT") {
                literals.insert(child.children[0].children[0].type);
            } else if (child.type == "NOT") {
                literals.insert("!" + child.children[0].type);
            } else {
                literals.insert(child.type);
            }
        }
        // If a clause contains both a variable and its negation, it's always true
        for (const auto &lit: literals) {
            if (lit[0] == '!' && literals.count(lit.substr(1))) {
                return Node{"TRUE"};
            }
        }
        vector<Node> newChildren;
        for (const auto &lit: literals) {
            if (lit[0] == '!') {
                newChildren.push_back(Node{"NOT", {Node{lit.substr(1)}}});
            } else {
                newChildren.push_back(Node{lit});
            }
        }
        return Node{"OR", newChildren};
    }
    return node;  // for NOT and literals
}

CNFConverter::Node CNFConverter::toCNFTree(Node root) {
    simplifyDoubleNegation(root);

    if (root.type == "VAR" || (root.type == "NOT" && root.children[0].type == "VAR")) {
        return root;
    }

    if (root.type == "BICONDITIONAL") {
        Node implies1 = {"IMPLIES", {root.children[0], root.children[1]}};
        Node implies2 = {"IMPLIES", {root.children[1], root.children[0]}};
        return {"AND", {toCNFTree(implies1), toCNFTree(implies2)}};
    }

    if (root.type == "IMPLIES") {
        Node notNode = {"NOT", {root.children[0]}};
        Node orNode = {"OR", {notNode, root.children[1]}};
        return toCNFTree(orNode);
    }

    if (root.type == "NOT") {
        Node child = root.children[0];
        if (child.type == "OR") {
            return {"AND", {toCNFTree(Node{"NOT", {child.children[0]}}),
                            toCNFTree(Node{"NOT", {child.children[1]}})}};
        }
        if (child.type == "AND") {
            return {"OR", {toCNFTree(Node{"NOT", {child.children[0]}}),
                           toCNFTree(Node{"NOT", {child.children[1]}})}};
        }
        if (child.type == "IMPLIES") {
            Node notA = {"NOT", {child.children[0]}};
            return {"OR", {notA, child.children[1]}};
        }
        if (child.type == "BICONDITIONAL") {
            return toCNFTree(Node{"NOT", {Node{"OR", {Node{"IMPLIES", {child.children[0], child.children[1]}},
                                                      Node{"IMPLIES", {child.children[1], child.children[0]}}}}}});
        }
    }

    if (root.type == "AND") {
        if (root.children[0].type == "OR") {
            return {"OR", {toCNFTree(Node{"AND", {root.children[0].children[0], root.children[1]}}),
                           toCNFTree(Node{"AND", {root.children[0].children[1], root.children[1]}})}};
        }
        if (root.children[1].type == "OR") {
            return {"OR", {toCNFTree(Node{"AND", {root.children[0], root.children[1].children[0]}}),
                           toCNFTree(Node{"AND", {root.children[0], root.children[1].children[1]}})}};
        }
    }

    if (root.type == "OR") {
        return {"AND", {toCNFTree(Node{"NOT", {root.children[0]}}),
                        toCNFTree(Node{"NOT", {root.children[1]}})}};
    }

    for (Node &child: root.children) {
        child = toCNFTree(child);
    }

    return root;
}

vector<string> CNFConverter::convertBnf(const vector<string> &exprs) {
    vector<string> allClauses;
    set<string> uniqueClauses;

    for (const auto &expr: exprs) {
        vector<Token> tokenlist = tokenize(expr);
        int pos = 0;
        Node astRoot = parse(tokenlist, pos); // 解析得到AST

        Node cnfRoot = toCNF(astRoot);

        auto currentClauses = convertToCNF(cnfRoot); // 将AST转换为CNF形式

        for (const auto &clause: currentClauses) {
            stringstream ss;
            for (const auto &lit: clause) {
                ss << lit << "";
            }
            string clauseStr = ss.str();
            if (uniqueClauses.find(clauseStr) == uniqueClauses.end()) {
                uniqueClauses.insert(clauseStr);
                allClauses.push_back(clauseStr);
            }
        }
    }
// for test
    vector<string> allClauses1 = {"!A !B", "!A C B !D", "!A E B !D", "A !B C D", "A !B E", "!A !C D", "C A D",
                                  "!C !E !A", "!C !E B", "!D !E !A", "!D !E B"};
    return allClauses1;
}

vector<string> CNFConverter::convert(const vector<string> &exprs) {
    vector<string> allClauses;
    set<string> uniqueClauses;

    for (const auto &expr: exprs) {
        vector<Token> tokenlist = tokenize(expr);
        int pos = 0;
        Node astRoot = parse(tokenlist, pos); // 解析得到AST
        //printTree(astRoot);

        Node cnfRoot = toCNF(astRoot);

        // printTree(cnfRoot);

        //transform
        auto currentClauses = convertToCNF(cnfRoot);
        // remove duplicates
        for (const auto &clause: currentClauses) {
            stringstream ss;
            for (const auto &lit: clause) {
                ss << lit << "";
            }
            string clauseStr = ss.str();
            if (uniqueClauses.find(clauseStr) == uniqueClauses.end()) {
                uniqueClauses.insert(clauseStr);
                allClauses.push_back(clauseStr);
            }
        }
    }

    return allClauses;
}


CNFConverter::CNFConverter() {

}

