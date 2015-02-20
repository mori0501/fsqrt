#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define INFILE "new_sqrt_table.txt"
#define TESTS 2000000
#define IS_TIME_RAND 3 // 1ならsrand(time(NULL)) 0でも1でもなければsrand(IS_TIME_RAND)する
//2 仮数部 all 1 だめ

typedef union u2f{
  uint32_t uuu;
  float fff;
}u2f;

uint32_t binarytouint(char *bin){
  uint32_t ret=0;
  uint32_t temp=1u;
  int i=0;
  for(i=0;i<32;i++){
    if(bin[31-i]=='1'){
      ret += temp << i;
    }
  }
  return ret;
}

char *uinttobinary(uint32_t ui){
  char *ret=(char*)malloc(sizeof(char)*50);
  int i;
  for(i=0;i<32;i++){
    if((ui >> i) & 1u){
      ret[31-i]='1';
    }
    else
      ret[31-i]='0';
  }
  ret[32]='\0';
  return ret;
}

char *lluinttobinary(long long unsigned int ui){
  char *ret=(char*)malloc(sizeof(char)*50);
  int i;
  for(i=0;i<36;i++){
    if((ui >> i) & 1u){
      ret[35-i]='1';
    }
    else
      ret[35-i]='0';
  }
  ret[36]='\0';
  return ret;
}

uint32_t downto(uint32_t i,int high,int low){
  int lsh = 31 - high;
  int rsh = lsh + low;
  i <<= lsh;
  i >>= rsh;
  return i;
}

long long unsigned int binarytoullint(char *bin){
  long long unsigned int ret=0;
  long long unsigned int temp=1;
  int i=0;
  for(i=0;i<36;i++){
    if(bin[35-i]=='1'){
      ret += temp << i;
    }
  }
  return ret;
}

uint32_t yllui2uint(long long unsigned int i){
  return i>>13;
}

uint32_t lowllui2uint(long long unsigned int i){
  i <<= 32;
  i >>= 32;
  return (uint32_t)i;
}

uint32_t make_ans(uint32_t sign,uint32_t exp,uint32_t mant){
  sign <<= 31;
  exp <<= 23;
  return sign | exp | mant;
}

int tlb_flag = 0;
static char tlb[2048][40];
void init_tlb(){
  int i = 0;
  FILE *fp = fopen(INFILE,"r");
  char str[40];
  while(fscanf(fp,"%s",str) != EOF){
    strcpy(tlb[i],str);
    i++;
  }
  fclose(fp);
}

uint32_t fsqrt(uint32_t i){
  if(tlb_flag == 0){
    init_tlb();
    tlb_flag = 1;
  }
  if(downto(i,30,0) == 0){
    return i;
  }
  if(downto(i,31,31) == 1){
    return 0xffc00000;
  }
  uint32_t sign = 0;
  uint32_t i_exp = downto(i,30,23);
  uint32_t is_odd = downto(i,23,23);
  uint32_t exp;
  uint32_t one = 127;
  if(i_exp >= one){
    exp = i_exp - one;
    exp >>= 1;
    exp += one;
  }else{
    exp = one - i_exp;
    exp += 1;
    exp >>= 1;
    exp = one - exp;
  }
  uint32_t index;
  if(is_odd == 1){
    index = downto(i,22,15);
  }else{
    index = downto(i,22,14) + 256;
  }
  long long unsigned int yd = binarytoullint(tlb[index]);
  uint32_t ydtemplow,y,ymant,d;
  ymant = yllui2uint(yd);
  y = (1 << 23) | ymant;
  ydtemplow = lowllui2uint(yd);
  d = (1 << 13) | downto(ydtemplow,12,0);
  uint32_t mant;
  if(is_odd == 1){
    mant = y - ((d * ((1 << 15) - (downto(i,14,0)))) >> 15);
  }else{
    if(downto(i,22,0) == (1 << 23) - 1){
      mant = downto(i,22,0);
    }else{
      mant = y - ((d * ((1 << 14) - (downto(i,13,0)))) >> 14);
    }
  }
  uint32_t ans = make_ans(sign,exp,downto(mant,22,0));
  return ans;
}

char *makebinary(){
  char *ret = (char*)malloc(sizeof(char)*50);
  int i;
  for(i=0;i<32;i++){
    int ra=rand()%2;
    if(i == 0 || i == 2 || i == 3 || i == 4 || i == 5 || i == 6 || i == 7){
      ret[i] = '0';
    }else if(i == 1){
      ret[i] = '1';
    }
    else{
      if(ra == 1)
	ret[i] = '1';
      else if(ra == 0)
	ret[i] = '0';
    }
  }
  ret[32] = '\n';
  for(i=33;i<50;i++){
    ret[i] = 0;
  }
  char temp[9];
  for(i=0;i<8;i++){
    temp[i] = ret[1+i];
  }
  temp[8] = 0;
  if(strcmp(temp,"00000000") == 0 || strcmp(temp,"11111111") == 0  || strncmp(ret,"1",1) == 0 ){
    free(ret);
    ret = makebinary();
  }
  return ret;
}
int main(int argc,char *argv[]){
  u2f ans,temp2;
  if(argc != 2){
    if(IS_TIME_RAND == 1){
      srand(time(NULL));
    }else if(IS_TIME_RAND != 0){
      srand((unsigned)IS_TIME_RAND);
    }
  }else{
    unsigned int time_rand = atoi(argv[1]);
    srand(time_rand);
  }
  int i;
  int count[17] = {0};
  int distance;
  int max=0,min=0;
  char my_maxstr[50]={0},my_minstr[50]={0};
  char ans_max[50]={0},ans_min[50]={0};
  char quest_max[50]={0},quest_min[50]={0};
  for(i=0;i<TESTS;i++){
    char *random = makebinary();
    uint32_t ux = binarytouint(random);
    ans.uuu = ux;
    uint32_t finv_answer = temp2.uuu = fsqrt(ux);
    //  printf("finv     : %s : %f\n",uinttobinary(finv_answer),temp2.ftemp);
    float fx = ans.fff;
    ans.fff = sqrtf(fx);
    distance = finv_answer - ans.uuu;
    if(distance <= -8){
      count[0]++;
    }
    else if(distance >= 8){
      count[16]++;
    }
    else{
      count[distance+8]++;
    }
    //if(distance > 500 || distance < -500)
    // continue; // exp が0x00または0xffのときおかしくなるので
    if(max < distance){
      max = distance;
      strcpy(my_maxstr,uinttobinary(finv_answer));
      strcpy(ans_max,uinttobinary(ans.uuu));
      strcpy(quest_max,random);
    }
    else if(min > distance){
      min = distance;
      strcpy(my_minstr,uinttobinary(finv_answer));
      strcpy(ans_min,uinttobinary(ans.uuu));
      strcpy(quest_min,random);
    }
    //printf("answer   : %s : %f\n",uinttobinary(temp.utemp),temp.ftemp);
    free(random);
  }
  for(i=0;i<17;i++){
    printf("count[%d] = %d\n",i-8,count[i]);
  }
  printf("指数部が0x00または0xffのとき除外している。\n");
  printf("max = %d\n",max);
  printf("fsqrt : %s\n",my_maxstr);
  printf("ansx  : %s\n",ans_max);
  printf("qmax  : %s",quest_max);
  printf("debug :  EXP     FRA    89\n\n");
  printf("min = %d\n",min);
  printf("fsqrt : %s\n",my_minstr);
  printf("sqrtf : %s\n",ans_min);
  printf("qmin  : %s\n",quest_min);
  printf("debug :  EXP     FRA    89 \n");
  return 0;
}

