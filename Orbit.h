#ifndef Orbit_h_DEFINED
#define Orbit_h_DEFINED

#define MAXSTACK 16
#define MAXVAR 16

void orbit(const char* script);
void orbit_registervar(const char* name,const char* value);
void orbit_registervar(const char* name,int value);
void orbit_registerfunction(const char* name,void(*func)(void));
char* gettoken();
bool orbit_isnumber(const char* var);
bool orbit_isstring(const char* var);
bool orbit_expectequal();
int orbit_popinteger();
char* orbit_getvariable(const char* name);
void orbit_pushinteger(int value);
char* orbit_popstack();
void orbit_pushstack(const char* value);
bool orbit_isfunction(const char* token);
bool orbit_isvar(const char* token);
bool orbit_isassign(const char* token);
void orbit_execute(const char* token);
void orbit_checkfunction();
void orbit_run();
void orbit_exit();


void orbit_digitalWrite();
void orbit_analogWrite();
void orbit_analogRead();
void orbit_pinMode();
void orbit_digitalRead();
void orbit_analogReference();
void orbit_delay();

struct __funclist__
{
    void(*func)(void);
    char* name;
    struct __funclist__ *next;
    static struct __funclist__ *end;
}*orbit_functions=NULL;
__funclist__* __funclist__::end=NULL;


char* orbit_stack[MAXSTACK];
int orbit_currentstack=0;

struct __var__
{
    char* name;
    char* value;
} orbit_var[MAXVAR];
int orbit_currentvar=0;

char *orbit_token;
char *orbit_script;
void orbit(const char* script)
{
    orbit_registerfunction("digitalWrite",orbit_digitalWrite);
    orbit_registerfunction("digitalRead",orbit_digitalRead);
    orbit_registerfunction("analogWrite",orbit_analogWrite);
    orbit_registerfunction("analogRead",orbit_analogRead);
    orbit_registerfunction("pinMode",orbit_pinMode);
    orbit_registerfunction("analogReference",orbit_analogReference);
    orbit_registerfunction("delay",orbit_delay);

    orbit_registervar("LOW",LOW);
    orbit_registervar("HIGH",HIGH);
    orbit_registervar("INPUT",INPUT);
    orbit_registervar("OUTPUT",OUTPUT);

    orbit_script = new char[strlen(script)+1];
    strcpy(orbit_script,script);
    orbit_token = strtok (orbit_script," ;");
}

void orbit_registerfunction(const char* name,void(*func)(void))
{
    __funclist__* node =  new __funclist__;
    node->name = new char[strlen(name)+1];
    strcpy(node->name,name);
    node->func = func;
    node->next = NULL;
    if(orbit_functions==NULL)
    {
        orbit_functions = node;
    }
    else
    {
        node->end->next = node;
    }
    node->end = node;
}
void orbit_registervar(const char* name,const char* value)
{
    for(int x = 0; x < orbit_currentvar;++x)
    {
        if(!strcmp(name,orbit_var[x].name))
        {
            delete[] orbit_var[x].value;
            orbit_var[x].value = new char[strlen(value)+1];
            strcpy(orbit_var[x].value,value);
            return;
        }
    }
    orbit_var[orbit_currentvar].name = new char[strlen(name)+1];
    strcpy(orbit_var[orbit_currentvar].name,name);
    orbit_var[orbit_currentvar].value = new char[strlen(value)+1];
    strcpy(orbit_var[orbit_currentvar].value,value);
    orbit_currentvar++;
}
void orbit_registervar(const char* name,int value)
{
    for(int x = 0; x < orbit_currentvar;++x)
    {
        if(!strcmp(name,orbit_var[x].name))
        {
            delete[] orbit_var[x].value;
            orbit_var[x].value = new char[4];
            itoa(value,orbit_var[x].value,10);
            return;
        }
    }
    orbit_var[orbit_currentvar].name = new char[strlen(name)+1];
    strcpy(orbit_var[orbit_currentvar].name,name);
    orbit_var[orbit_currentvar].value = new char[4];
    itoa(value,orbit_var[orbit_currentvar].value,10);
    orbit_currentvar++;
}
char* gettoken()
{
    return strtok (NULL," ;\n");
}
bool orbit_isnumber(const char* var)
{
    if(var==NULL)
        return false;
    const int size = strlen(var);
    for(int x = 0; x < size; ++x)
        if( var[x] < '0' || var[x] > '9')
            return false;
    return true;
}

bool orbit_isstring(const char* var)
{
    const int size = strlen(var);
    if( var[0] == '"' && var[size-1] > '"')
        return true;
    return false;
}
bool orbit_expectequal()
{
    char* token = gettoken();
    if(!strcmp(token,"="))
        return true;
    return false;
}
int orbit_popinteger()
{
    int value = 0;
    if(orbit_currentstack-1<0)
    {
        Serial.write("error: invalid stack index\n");
        return 0;
    }
    char* token = orbit_popstack();
    if(orbit_isnumber(token))
    {
        value = atoi(token);
        delete[] token;
    }
    else
        Serial.write("error: not a number");
    return value;
}
char* orbit_getvariable(const char* name)
{
    for(int x = 0; x < orbit_currentvar; ++x)
    {
        if(!strcmp(orbit_var[x].name,name))
        {
            return orbit_var[x].value;
        }
    }
    return NULL;
}
void orbit_pushinteger(int value)
{
    orbit_stack[orbit_currentstack] = new char[4];
    itoa(value,orbit_stack[orbit_currentstack],10);
    orbit_currentstack++;
}
char* orbit_popstack()
{
    int index = orbit_currentstack-1;

    char* tmp = new char[strlen(orbit_stack[index])+1];
    strcpy(tmp,orbit_stack[index]);
    delete[] orbit_stack[index];

    orbit_currentstack--;
    return tmp;
}
void orbit_pushstack(const char* value)
{
    if(orbit_currentstack>MAXSTACK)
        Serial.write("error: stack overflow");
    orbit_stack[orbit_currentstack] = new char[strlen(value)+1];
    strcpy(orbit_stack[orbit_currentstack],value);
    orbit_currentstack++;
}
bool orbit_isfunction(char* token)
{
    if(token==NULL)
        return false;
    __funclist__* func = orbit_functions;
    while(func!=NULL)
    {
        if(!strcmp(token,func->name))
        {
            return true;
        }
        func = func->next;
    }
    return false;
}
bool orbit_isvar(const char* token)
{
    if(token==NULL)
        return false;
    if(!strcmp(token,"var"))
        return true;
    return false;
}
bool orbit_isassign(const char* token)
{
    if(!strcmp(token,"="))
        return true;
    return false;
}

void orbit_execute(const char* token)
{
    __funclist__* func = orbit_functions;
    while(func!=NULL)
    {
        if(!strcmp(token,func->name))
        {
            func->func();
            return;
        }
        func = func->next;
    }
}

void orbit_checkfunction()
{
    char *tfunction = orbit_token;
    ///adiciona os parametros na stack
    while((orbit_token = gettoken())!=NULL && !orbit_isfunction(orbit_token) &&strcmp(orbit_token,"var"))
    {
        if(orbit_isnumber(orbit_token) || orbit_isstring(orbit_token))
            orbit_pushstack(orbit_token);
        else
            orbit_pushstack(orbit_getvariable(orbit_token));
    }
    orbit_execute(tfunction);
}
void orbit_run()
{
    ///Pega o proximo token
    do
    {
        ///se for funcao, executa
        while(orbit_isfunction(orbit_token))
        {
            orbit_checkfunction();
        }
        orbit_currentstack = 0;
        if(orbit_isvar(orbit_token))
        {
            if((orbit_token = gettoken())!=NULL)
            {
                ///se for uma atribuicao
                if(orbit_expectequal())
                {
                    char *vartoken = orbit_token;
                    ///pega o proximo token
                    if((orbit_token = gettoken())!=NULL)
                    {
                        ///se for funcao executa e pega o valor da stack
                        while(orbit_isfunction(orbit_token))
                        {
                            orbit_checkfunction();
                            orbit_registervar(vartoken,orbit_popstack());
                        }
                        ///se for numero atribui na variavel
                        if(orbit_isnumber(orbit_token))
                        {
                            orbit_registervar(vartoken,orbit_token);
                        }

                    }
                    else
                    {
                        Serial.write("error: expected value");
                    }
                }
            }

        }

    }while((orbit_token = gettoken())!=NULL);
    orbit_exit();
}
void orbit_exit()
{
    while(orbit_functions)
    {
        __funclist__* func = orbit_functions->next;
        delete[] orbit_functions->name;
        delete orbit_functions;
        orbit_functions = func;
    }
    delete[] orbit_script;
}

void orbit_digitalWrite()
{
    int value = orbit_popinteger();
    int pin = orbit_popinteger();
    digitalWrite(pin, value);
}
void orbit_analogWrite()
{
    int value = orbit_popinteger();
    int pin = orbit_popinteger();
    analogWrite(pin, value);
}
void orbit_analogRead()
{
    int pin = orbit_popinteger();
    int value = analogRead(pin);
    orbit_pushinteger(value);
}
void orbit_digitalRead()
{
    int pin = orbit_popinteger();
    int value = digitalRead(pin);
    orbit_pushinteger(value);
}
void orbit_delay()
{
    int value = orbit_popinteger();
    delay(value);
}
void orbit_pinMode()
{
    int mode = orbit_popinteger();
    int pin = orbit_popinteger();
    pinMode(pin, mode);
}
void orbit_analogReference()
{
    int mode = orbit_popinteger();
    analogReference(mode);
}

#endif

