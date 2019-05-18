#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  if(argc != 2)
  {
    fprintf(stderr,"引数の個数が正しくありません");
    return 1;
  }

	//入力された引数を
	char *p = argv[1];

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  printf(" mov rax,%ld\n",strtol(p,&p,10));

	while(*p)
	{
		//+を見つけた位置から数値の終わりまでを10進数に変換して、次のポインタの位置を返す
		if(*p == '+')
		{
			//+の次の文字(数値の部分から読み込みを始める)
			p++;

			//+にかかっている数値部分をすべて読み込み
			printf("	add rax, %ld\n",strtol(p,&p,10));

			continue;
		}
		if(*p == '-')
		{
			p++;

			printf("	sub rax,%ld\n",strtol(p,&p,10));

			continue;
		}

		fprintf(stderr,"予期しない文字です: '%c'\n", *p);
		return 1;
	}
	




  printf(" ret\n");

  return 0;
}
