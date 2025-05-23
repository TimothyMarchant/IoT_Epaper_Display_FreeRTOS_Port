#ifndef maxdigits_short
#define maxdigits_short 5
#endif
#ifndef zeroasciivalue
#define zeroasciivalue 48
#endif
void parseshort(unsigned char* digits,unsigned short number){
    unsigned short temp=1;
    for (unsigned char i=0;i<maxdigits_short;i++){
        digits[maxdigits_short-i-1]=(unsigned char) (((number/temp)%10)+zeroasciivalue);
        temp*=10;
    }
}
//it is assumed that the user 
void AddParsedShortToString(unsigned char* digits,char* string){
    
}