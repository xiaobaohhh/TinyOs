/*
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-09 13:02:24
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-24 19:53:35
 * @FilePath: \start\source\shell\main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "lib_syscall.h"
#include <stdio.h>
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/file.h>
#include "fs/file.h"
#include "dev/tty.h"
static cli_t cli;
static const char * prompt = "shell>>";

static int do_help(int argc,char **argv)
{
    for(const cli_cmd_t * cmd = cli.cmd_start;cmd < cli.cmd_end;cmd++)
    {
        printf("%s - %s\n",cmd->name,cmd->usage);
    }
    return 0;
}
static int do_clear(int argc,char **argv)
{
    printf("%s",ESC_CLEAR_SCREEN);
    printf("%s",ESC_MOVE_CURSOR(0,0));
    return 0;
}
static int do_echo(int argc,char **argv)
{
    if(argc == 1)
    {
        char msg_buf[128];
        fgets(msg_buf,sizeof(msg_buf),stdin);
        msg_buf[strlen(msg_buf) - 1] = '\0';
        puts(msg_buf);
        return 0;
    }
    int count = 1;
    char ch;
    while((ch = getopt(argc,argv,"n:h")) != -1)
    {
        switch(ch)
        {
            case 'n':
                count = atoi(optarg);
                break;
            case 'h':
                printf("echo [-n count] message\n");
                optind = 1;
                return 0;
            case '?':
                if(optarg)
                {
                    fprintf(stderr,ESC_COLOR_ERROR"%s: invalid option -- '%c'\n"ESC_COLOR_DEFAULT,argv[0],optarg);
                }
                optind = 1;
                return -1;
            default:
                optind = 1;
                return -1;
        }
    }
    if(optind > argc - 1)
    {
        fprintf(stderr,ESC_COLOR_ERROR"Message is empty\n"ESC_COLOR_DEFAULT);
        optind = 1;
        return -1;
    }
    char * msg = argv[optind];
    for(int i = 0;i < count;i++)
    {
        puts(msg);
    }
    optind = 1;
    return 0;
}
static int do_exit(int argc,char **argv)
{
    exit(0);
    return 0;
}

static int do_ls(int argc,char **argv)
{
    DIR * p_dir = opendir("/home");
    if(!p_dir)
    {
        printf("open dir failed\n");
        return -1;
    }
    struct dirent * entry;
    while((entry = readdir(p_dir)) != NULL)
    {
        printf("%c %s %d\n",entry->type == FILE_DIR ? 'd' : 'f',entry->name,entry->size);
    }
    closedir(p_dir);
    return 0;
}

static int do_less(int argc,char **argv)
{
    int line_mode = 0;
    int count = 1;
    char ch;
    while((ch = getopt(argc,argv,"lh")) != -1)
    {
        switch(ch)
        {   
            case 'l':
                line_mode = 1;
                break;
            case 'n':
                count = atoi(optarg);
                break;
            case 'h':
                printf("less [-l] file\n");
                optind = 1;
                return 0;
            case '?':
                if(optarg)
                {
                    fprintf(stderr,ESC_COLOR_ERROR"%s: invalid option -- '%c'\n"ESC_COLOR_DEFAULT,argv[0],optarg);
                }
                optind = 1;
                return -1;
            default:
                optind = 1;
                return -1;
        }
    }
    if(optind > argc - 1)
    {
        fprintf(stderr,ESC_COLOR_ERROR"no file\n"ESC_COLOR_DEFAULT);
        optind = 1;
        return -1;
    }
    FILE * fp = fopen(argv[optind],"r");
    if(fp == NULL)
    {
        fprintf(stderr,ESC_COLOR_ERROR"open file failed\n"ESC_COLOR_DEFAULT);
        optind = 1;
        return -1;
    }
    char * buf = (char *)malloc(255);
    if(line_mode == 0)
    {
        while(fgets(buf,255,fp) != NULL)
        {
            fputs(buf,stdout);
        }
    }
    else
    {
        setvbuf(stdin,NULL,_IONBF,0);
        ioctl(0,TTY_CMD_ECHO,0,0);
        while(1)
        {
            char * b = fgets(buf,255,fp);
            if(b == NULL)
            {
                break;
            }
            fputs(buf,stdout);
            int ch;
            while((ch = fgetc(stdin)) != 'n')
            {
                if(ch == 'q')
                {
                    goto less_quit;
                }
            }
        }
less_quit:
        setvbuf(stdin,NULL,_IOLBF,BUFSIZ);
        ioctl(0,TTY_CMD_ECHO,1,0);
    }
    free(buf);
    fclose(fp);

    optind = 1;
    return 0;
}
static int do_cp(int argc,char **argv)
{
    if(argc < 3)
    {
        fprintf(stderr,ESC_COLOR_ERROR"cp src dest\n"ESC_COLOR_DEFAULT);
        return -1;
    }
    FILE * from,* to;
    from = fopen(argv[1],"rb");
    to = fopen(argv[2],"wb");
    if(!from || !to)
    {
        fprintf(stderr,ESC_COLOR_ERROR"open file failed\n"ESC_COLOR_DEFAULT);
        goto cp_failed;
    }
    const int buf_size = 255;
    char * buf = (char * )malloc(buf_size);
    int size;
    while((size = fread(buf,1,buf_size,from)) > 0)
    {
        fwrite(buf,1,size,to);
    }
    free(buf);
cp_failed:
    if(from)
    {
        fclose(from);
    }
    if(to)
    {
        fclose(to);
    }
    return 0;
}
static int do_rm(int argc,char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr,ESC_COLOR_ERROR"no file\n"ESC_COLOR_DEFAULT);
        return -1;
    }
    int err = unlink(argv[1]);
    if(err < 0)
    {
        fprintf(stderr,ESC_COLOR_ERROR"unlink failed\n"ESC_COLOR_DEFAULT);
        return err;
    }
    return 0;
}
static const cli_cmd_t cmd_list[] = {
    {"help","show help",do_help},
    {"clear","clear screen",do_clear},
    {"echo","echo [-n count] message",do_echo},
    {"quit","quit shell",do_exit},
    {"ls","list directory",do_ls},
    {"less","less file",do_less},
    {"cp","cp src dest",do_cp},
    {"rm","rm file",do_rm},
};

static const cli_cmd_t * find_builtin(const char * name)
{
    for(const cli_cmd_t * cmd = cli.cmd_start;cmd < cli.cmd_end;cmd++)
    {
        if(strcmp(cmd->name,name) == 0)
        {
            return cmd;
        }
    }
    return NULL;
}

static int run_builtin(const cli_cmd_t * cmd,int argc,char **argv)
{
    return cmd->do_func(argc,argv);
}

static void cli_init(const char * prompt,const cli_cmd_t * cmd_list,int size)
{
    cli.prompt = prompt;
    memset(cli.curr_input,0,CLI_INPUT_SIZE);
    cli.cmd_start = cmd_list;
    cli.cmd_end = cmd_list + size;
}

static void show_prompt()
{
    printf("%s",cli.prompt);
    fflush(stdout);
}

static const char * find_exec_path(const char * name)
{
    int fd = open(name,O_RDONLY);
    if(fd < 0)
    {
        return NULL;
    }
    close(fd);
    return name;
}


static void run_exec_file(const char * path,int argc,char **argv)
{
    int pid = fork();
    if(pid < 0)
    {
        fprintf(stderr,"fork failed %s\n",path);
        return;
    }
    else if(pid == 0)
    {
        int err = execve(path,argv,(char **)0);
        if(err < 0)
        {
            fprintf(stderr,"execve failed %s\n",path);
            exit(-1);
        }
    }
    else
    {
        int status;
        int ret = wait(&status);
        printf("ret = %d,status = %d\n",ret,status);
    }
}
int main(int argc,char **argv)
{
    int fd = open(argv[0],O_RDWR);
    dup(fd);
    dup(fd);
    
    cli_init(prompt,cmd_list,sizeof(cmd_list)/sizeof(cmd_list[0]));
    for(;;)
    {
        show_prompt();
        //char * str = fgets(cli.curr_input,CLI_INPUT_SIZE,stdin);
        char * str = gets(cli.curr_input);
        if(!str)
        {
            continue;
        }
        char * cr = strchr(str,'\n');
        if(cr)
        {
            *cr = '\0';
        }
        cr = strchr(str,'\r');
        if(cr)
        {
            *cr = '\0';
        }
        int argc = 0;
        char * argv[CLI_MAX_ARG_COUNT];
        memset(argv,0,sizeof(argv));
        const char * space = " ";
        char * token = strtok(str,space);
        while(token)
        {
            argv[argc++] = token;
            token = strtok(NULL,space);
        }
        if(argc == 0)
        {
            continue;
        }
        const cli_cmd_t * cmd = find_builtin(argv[0]);
        if(cmd)
        {
            run_builtin(cmd,argc,argv);
            continue;
        }
        const char * path = find_exec_path(argv[0]);
        if(path)
        {
            run_exec_file(path,argc,argv);
            continue;
        }
        else
        {
            fprintf(stderr,ESC_COLOR_ERROR"%s: command not found\n"ESC_COLOR_DEFAULT,cli.curr_input);
        }
        fprintf(stderr,ESC_COLOR_ERROR"%s: command not found\n"ESC_COLOR_DEFAULT,cli.curr_input);
    }
}