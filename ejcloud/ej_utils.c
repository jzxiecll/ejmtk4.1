#include "ej_utils.h"
#include <time.h>
void xor(uint8_t *buf, uint8_t size)
{
	uint8_t i = 0;
	for(i = 0; i < size; i++) {
		buf[i] ^= 0x5A;
	}
}


int ChangeStr2Hex(char s[],char bits[]) {
    int i,n = 0;
    for(i = 0; s[i]; i += 2) {
        if(s[i] >= 'A' && s[i] <= 'F')
            bits[n] = s[i] - 'A' + 10;
        else bits[n] = s[i] - '0';
        if(s[i + 1] >= 'A' && s[i + 1] <= 'F')
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'A' + 10);
        else bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
		//wmprintf("%02X ",bits[n]);
        ++n;
    }
    return n;
}


void ChangeHex2Str(const char *sSrc,  char *sDest, int nSrcLen)
{
    int  i;
    char szTmp[3];
    for( i = 0; i < nSrcLen; i++ )
    {
        sprintf( szTmp, "%02x", (unsigned char) sSrc[i] );
        memcpy( &sDest[i * 2], szTmp, 2 );
    }
    return ;
}


int ChangeStrHex(char s[],char bits[]) {
    int i,n = 0;
    for(i = 0; s[i]; i += 2) {
        if(s[i] >= 'a' && s[i] <= 'f')
            bits[n] = s[i] - 'a' + 10;
        else bits[n] = s[i] - '0';

        if(s[i + 1] >= 'a' && s[i + 1] <= 'f')
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'a' + 10);
        else bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
		EJ_Printf("%02X ",bits[n]);
        ++n;
    }
    return n;
}



static const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="; 
 
static char find_pos(char ch)   
{ 
    char *ptr = (char*)strrchr(base, ch);//the last position (the only) in base[] 
    return (ptr - base); 
} 



char *base64_encode(const char* data, int data_len) 
{ 

    int prepare = 0; 
    int ret_len; 
    int temp = 0; 
    char *ret = NULL; 
    char *f = NULL; 
    int tmp = 0; 
    char changed[4]; 
    int i = 0; 
    ret_len = data_len / 3; 
    temp = data_len % 3; 
    if (temp > 0) 
    { 
        ret_len += 1; 
    } 
    ret_len = ret_len*4 + 1; 
    ret = (char *)EJ_mem_malloc(ret_len); 
      
    if ( ret == NULL) 
    { 
        EJ_Printf("No enough memory.\n"); 
        return 0; 
    } 
    memset(ret, 0, ret_len); 
    f = ret; 
    while (tmp < data_len) 
    { 
        temp = 0; 
        prepare = 0; 
        memset(changed, '\0', 4); 
        while (temp < 3) 
        { 
            //printf("tmp = %d\n", tmp); 
            if (tmp >= data_len) 
            { 
                break; 
            } 
            prepare = ((prepare << 8) | (data[tmp] & 0xFF)); 
            tmp++; 
            temp++; 
        } 
        prepare = (prepare<<((3-temp)*8)); 
        //printf("before for : temp = %d, prepare = %d\n", temp, prepare); 
        for (i = 0; i < 4 ;i++ ) 
        { 
            if (temp < i) 
            { 
                changed[i] = 0x40; 
            } 
            else 
            { 
                changed[i] = (prepare>>((3-i)*6)) & 0x3F; 
            } 
            *f = base[changed[i]]; 
            //printf("%.2X", changed[i]); 
            f++; 
        } 
    } 
    *f = '\0'; 
      
    return ret; 
      
} 
/* */ 

/* */ 
char *base64_decode(const char *data, int data_len) 
{ 
    int ret_len = (data_len / 4) * 3; 
    int equal_count = 0; 
    char *ret = NULL; 
    char *f = NULL; 
    int tmp = 0; 
    int temp = 0; 
    char need[3]; 
    int prepare = 0; 
    int i = 0; 
    if (*(data + data_len - 1) == '=') 
    { 
        equal_count += 1; 
    } 
    if (*(data + data_len - 2) == '=') 
    { 
        equal_count += 1; 
    } 
    if (*(data + data_len - 3) == '=') 
    {//seems impossible 
        equal_count += 1; 
    } 
    switch (equal_count) 
    { 
		    case 0: 
		        ret_len += 4;//3 + 1 [1 for NULL] 
		        break; 
		    case 1: 
		        ret_len += 4;//Ceil((6*3)/8)+1 
		        break; 
		    case 2: 
		        ret_len += 3;//Ceil((6*2)/8)+1 
		        break; 
		    case 3: 
		        ret_len += 2;//Ceil((6*1)/8)+1 
		        break; 
    } 
    ret = (char *)EJ_mem_malloc(ret_len); 
    if (ret == NULL) 
    { 
        EJ_Printf("No enough memory.\n"); 
        return 0; 
    } 
    memset(ret, 0, ret_len); 
    f = ret; 
    while (tmp < (data_len - equal_count)) 
    { 
        temp = 0; 
        prepare = 0; 
        memset(need, 0, 4); 
        while (temp < 4) 
        { 
            if (tmp >= (data_len - equal_count)) 
            { 
                break; 
            } 
            prepare = (prepare << 6) | (find_pos(data[tmp])); 
            temp++; 
            tmp++; 
        } 
        prepare = prepare << ((4-temp) * 6); 
        for (i=0; i<3 ;i++ ) 
        { 
            if (i == temp) 
            { 
                break; 
            } 
            *f = (char)((prepare>>((2-i)*8)) & 0xFF); 
            f++; 
        } 
    } 
    *f = '\0'; 
    return ret; 
}


int strsplinum(char *str, const char*del)   //判断总共有多少个分隔符，目的是在main函数中构造相应的arr指针数组
{
	 char *first = NULL;
	 char *second = NULL;
	 int num = 0;
	 first = strstr(str,del);
	 while(first != NULL)
	 {
	 second = first+1;
	 num++;
	 first = strstr(second,del);
	 }
	 return num;
}



void split( char **arr, char *str, const char *del)//字符分割函数的简单定义和实现
{
	char *s =NULL; 
	static char *psplitTmp = NULL;
	s=strtok_r(str,del,&psplitTmp);
	while(s!=NULL)
	{
		*arr++ = s;
		s = strtok_r(NULL,del,&psplitTmp);
	}
}




#define EJ_SLIM_UDIV_R(N, D, R) (((R)=(N)%(D)), ((N)/(D)))

void ej_itoa(char **buf, unsigned int  i, unsigned int base)
{
    char *s;
#define LEN   20
    unsigned int rem;
    static char rev[LEN + 1];

    rev[LEN] = 0;
    if (i == 0) {
        (*buf)[0] = '0';
        ++(*buf);
        return;
    }
    s = &rev[LEN];
    while (i) {
        i = EJ_SLIM_UDIV_R(i, base, rem);
        if (rem < 10) {
            *--s = rem + '0';
        } else if (base == 16) {
            *--s = "abcdef"[rem - 10];
        }
    }
    while (*s) {
        (*buf)[0] = *s++;
        ++(*buf);
    }
}



int getweekofyear()
{
	char s_month[5];
    int  month, day, year;	
    const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
    month = (strstr(month_names, s_month)-month_names)/3;
	return  (month*30+day)/7+1;
}


int getyearofyear()
{
	char s_month[5];
    int  day, year;
    sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
	return year%100;
}


