#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  トークンの種類
typedef enum {
    TK_RESERVED,  // 記号
    TK_NUM,       // 整数トークン
    TK_EOF,       // 入力終わりを表すトークン
} TokenKind;

//  Token構造体内でTokenを使うための前方宣言
typedef struct Token Token;

struct Token {
    TokenKind kind;  //  トークンの型
    Token *next;     //  次の入力トークン
    int val;         //  kindがTK_NUMの場合の、その数値
    char *str;       //  トークン文字列
};

Token *currentToken;

//  エラーを報告するための関数
//  printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

//  次のトークンが期待している記号のときには、トークンを1つ読み進めて心の値を返す
//  それ以外の場合は偽を返す
//  op : 期待する記号
bool consume(char op) {
    //  カレントトークンが記号以外か指定された記号でない場合
    if (currentToken->kind != TK_RESERVED || currentToken->str[0] != op) {
        return false;
    }

    //  トークンを次に進める
    currentToken = currentToken->next;

    //  期待した記号だったのでそれを返す
    return true;
}

void expect(char op){
  if(currentToken->kind != TK_RESERVED || currentToken->str[0] != op){
    error("expected %c",op);
  }
  currentToken = currentToken->next;
}

//  次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す
//  それ以外の場合にはエラーを報告する
int expect_number() {
    //  カレントトークンが数ではない場合
    if (currentToken->kind != TK_NUM) {
        error("数ではありません");
    }

    //  数値であることが確定しているので、数値を取得
    int val = currentToken->val;

    //  トークンを次へ進める
    currentToken = currentToken->next;

    return val;
}

//  終了文字か
bool at_eof() { return currentToken->kind == TK_EOF; }

//  新しいトークンを作成してcurにつなげる
//  kind : 作成するトークンの型
//  *cur : 作成するトークンの前のトークンのポインタ
//  *str : 作成するトークンの文字列
Token *new_token(TokenKind kind, Token *cur, char *str) {
    //  トークンを生成
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    //  カレント
    cur->next = tok;
    return tok;
}

//  入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        //  空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        //  記号の場合
        if (*p == '+' || *p == '-') {
            //  ここわかりにくいけどC言語の後置インクリメントだから、関数内に渡されるのは記号の文字
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        //  数値の場合
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        //  それ以外は現状トークナイズできないのでエラー
        error("トークナイズできません");
    }

    //  終了文字
    new_token(TK_EOF, cur, p);

    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    //  トークナイズする
    currentToken = tokenize(argv[1]);

    //  アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    //  整数なので式の最初は必ず整数なはず
    printf(" mov rax, %d\n", expect_number());

    //  終了文字が来るまでループ
    while (!at_eof()) {
        if (consume('+')) {
            printf("  add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }

    printf(" ret\n");

    printf(".section .note.GNU-stack,\"\",@progbits\n");

    return 0;
}
