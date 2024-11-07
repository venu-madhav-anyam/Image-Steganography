#if 0

#include <stdio.h>

void encode(char s[], char c)
{
    for(int i=0;s[i]!='\0';i++)
    {
        s[i] &= ~(1);
        
        int k = c & (128>>i);
        
        k>>=(7-i);
        
        s[i] |= k;
    }
}


int main()
{
    char c='A';
    char s[9] = "Hello Hi";
    
    printf("Before : %s\n",s);
    
    for(int j=0;s[j]!='\0';j++)
    {
        printf("\n-");
        
        for(int i=7;i>=0;i--)
        {
            int res = (s[j]>>i)&1;
        
            printf(" %d",res);
        }
        
    }
    
    encode(s,c);

    printf("\n");

    printf("After : %s\n",s);
    
    for(int j=0;s[j]!='\0';j++)
    {
        printf("\n-");
        
        for(int i=7;i>=0;i--)
        {
            int res = (s[j]>>i)&1;
        
            printf(" %d",res);
        }
    }
    
    
}

#endif

#if 0

#include <stdio.h>
#include<string.h>

void encode(char buffer[], char magic[])
{
    int m=0;
    for(int j=0;j<(strlen(magic));j++)
    {
        for(int i=0;i<8;i++)
        {
            buffer[m] &= ~(1);

            int k = magic[j] & (128>>i);
            
            k>>=(7-i);
            
            buffer[m++] |= k;
        }
    }
}


int main()
{
    char c[]="#*";
    unsigned char s[] = "07 00  06 03 00 06 03 00 09 06 02 0b 08 06 0f 0c";
    
    printf("Before : %s\n",s);
    
    for(int j=0;s[j]!='\0';j++)
    {
        printf("\n-");
        
        for(int i=7;i>=0;i--)
        {
            int res = (s[j]>>i)&1;
        
            printf(" %d",res);
        }
        
    }
    
    encode(s,c);

    printf("\n");

    printf("After : %s\n",s);
    
    for(int j=0;s[j]!='\0';j++)
    {
        printf("\n-");
        
        for(int i=7;i>=0;i--)
        {
            int res = (s[j]>>i)&1;
        
            printf(" %d",res);
        }
    }
    
    
}

#endif