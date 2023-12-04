
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "command.h"
#include <ctime>
/*	Global Variables	*/
char LOG_FILE_NAME[] = "/child-log.txt";
char *home_dir = getenv("HOME");
char *path_to_current_directory[128];
int next_dir = 0;
FILE *fp;

void ignoreCtrlC(int Signal);
void add_dir_to_path(char *directory);
int changeCurrentDirectory();
void handleSIGCHLD(int sig_num);
void catchSIGINT(int sig_num);
void openLogFile();
void closeLogFile();
void removeNewline(char *str, int size);





SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
	//_arguments is a statment
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	// el akheer by act as sentinal to indicate end of array
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}
	//-------------------------------------------
	int defaultIn = dup(0);
	int defaultOut = dup(1);
	int ip, op, err;
	
	if (_errFile) {
        	int err = open(_errFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        	dup2(err, 2);
        	close(err);
    	}
	if (_inputFile)
	{
		ip = open(_inputFile, O_RDONLY, 0777);
	}
	if (_outFile) {
        	if (!_append) {
       	     op = open(_outFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
       	 } else {
       	     op = open(_outFile, O_WRONLY | O_APPEND, 0777);
       	 }
 	   }

	
	//------------------------------
	printf("AHMAAA number of simple commands: %d\n", _numberOfSimpleCommands);
	// Print contents of Command data structure
	print();

	int foo = 0;
	//#################  llta2keed w omoor el debug ####################
	for(int j = 0; j < _numberOfSimpleCommands; j++){
		for(int i = 0; i < _simpleCommands[j]->_numberOfArguments; i++){
			printf("AHMAAA simple command %d argument %d: %s\n", j, i, _simpleCommands[j]->_arguments[i]);
		}
	}
	//##################################################################
	// Add execution here
	int fd[_numberOfSimpleCommands][2];
	while (foo < _numberOfSimpleCommands){
		if (strcmp(_simpleCommands[foo]->_arguments[0], "cd") == 0)
			{
				printf("\n");
				if (changeCurrentDirectory() == -1)
					printf("\033[31mError occurred. Make sure the directory you entered is valid\033[0m\n");
				continue;
			}

	// if there is input file change
	pipe(fd[foo]);
	if (foo == 0){
		if (_inputFile)
		{
			dup2(ip, 0);
			close(ip);
		}
		else
			dup2(defaultIn, 0);
		}	
	else
	{
		dup2(fd[foo - 1][0], 0);
		close(fd[foo - 1][0]);
	}
	if (foo == _numberOfSimpleCommands - 1)
	{
		if (_outFile)
			dup2(op, 1);
		else
			dup2(defaultOut, 1);
	}
	else
	{
		dup2(fd[foo][1], 1);
		close(fd[foo][1]);
	}
	// if there is output file change
			
		//if (_outFile)
		//	dup2(op, 1);
		//else
			//dup2(defaultOut, 1);
	int pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(2);
	}

	else if (pid == 0)
	{
		printf("Child process\n");
		printf("foo: %d\n", foo) ; 
		printf("el3ayzeeno %s \n", _simpleCommands[foo]->_arguments[0]);
		execvp(_simpleCommands[foo]->_arguments[0], _simpleCommands[foo]->_arguments);
		printf("execvp failed\n");
		
	}
		else
	{
		signal(SIGCHLD, handleSIGCHLD);
		dup2(defaultIn, 0);
		dup2(defaultOut, 1);
		if (!_background)
		waitpid( pid, 0, 0 );
		foo++;
		printf("Parent process\n");
	}
	}

	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec

	// Clear to prepare for next command
	clear();

	// Print new prompt
	prompt();
}

// Shell implementation
void ignoreCtrlC(int Signal)
{
	printf("\nCtrl+C is suppresed.Type exit to terminate \n");
	Command::prompt();
}

void
Command::prompt()
{
	signal(SIGINT, catchSIGINT);
	printf("my(Hamada)shell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);
void add_dir_to_path(char *directory)
{
	if (directory == NULL)
		next_dir = 0;
	else if (strcmp(directory, "..") == 0)
		next_dir--;
	else
		path_to_current_directory[next_dir++] = strdup(directory);
}
int changeCurrentDirectory()
{
	int returnValue;
	char *path = Command::_currentSimpleCommand->_arguments[1];

	if (path)
		returnValue = chdir(path);
	else
		returnValue = chdir(home_dir);

	if (returnValue == 0 || !path)
		add_dir_to_path(path);

	Command::_currentCommand.clear();
	return returnValue;
}
void removeNewline(char *str, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (str[i] == '\n')
		{
			str[i] = '\0';
			return;
		}
	}
}
void handleSIGCHLD(int sig_num)
{
	int status;
	wait(&status);
	openLogFile();
	flockfile(fp);
	time_t TIMER = time(NULL);
	tm *ptm = localtime((&TIMER));
	char currentTime[32];
	strcpy(currentTime, asctime(ptm));
	removeNewline(currentTime, 32);
	fprintf(fp, "%s: Child Terminated\n", currentTime);
	funlockfile(fp);
	fclose(fp);
	signal(SIGCHLD, handleSIGCHLD);
}
void catchSIGINT(int sig_num)
{
	signal(SIGINT, catchSIGINT);
	Command::_currentCommand.clear();
	printf("\r\033[0J"); // Erase myshell> ^C
	Command::_currentCommand.prompt();
	fflush(stdout);
}
void openLogFile() {
	char path_to_log[64];
	strcpy(path_to_log, getenv("HOME"));
	strcat(path_to_log, LOG_FILE_NAME);
	fp = fopen(path_to_log, "a");


	
}
void closeLogFile() {
	fclose(fp);
}
int 
main()
{
	signal(SIGINT,ignoreCtrlC);
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

