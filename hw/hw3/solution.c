#include<stdio.h>

int a[10],b[12];
int len1 = 10,len2=12;
int search(int index1,int index2){
    if(index1 == len1) return 0;
    if(index2 == len2) return 0;
    if(a[index1] == b[index2]) return 1+search(index1+1,index2+1);
    else{
        return 0;
    }
}

int func(){
    int res = 0;
    for(int i=0;i<len1;i++){
        for(int j=0;j<len2;j++){
            int temp = search(i,j);
            if(temp>res){
                res = temp;
            }
        }
    }
    return res;
}


int main(){
    int i;
  for(i=0;i<len1;i++){
      int t;
      scanf("%d",&t);
      a[i] = t;
  }
    for(i=0;i<len2;i++){
        int t;
        scanf("%d",&t);
        b[i] = t;
    }
    int res = func();
    printf("%d",res);
    return 0;
}