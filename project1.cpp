//Mark Tawara mht35915@uga.edu
//project1.cpp v1.0
//Create a Text Editor with Menu options

//NOTE: If things are not visible on the screen, try running again full-screen

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <menu.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))


using std::cout;
using std::endl;
using std::string;
using std::ofstream;
using std::stringstream;

char const *menu[] = {
		"Open",
		"Save",
		"Save As",
		"Exit",
	};

WINDOW *create_newwin(int height, int width, int starty, int startx){
	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	box(local_win, 0, 0);
	wrefresh(local_win);
	return local_win;
}

void destroy_win(WINDOW *local_win){
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color){
	int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}

int main(const int argc, const char * argv []){

	//Create items & Menu below
	ITEM **my_items;
	MENU *my_menu;
	int n_choices, i;
	n_choices = 4;
	my_items = (ITEM **)calloc(4, sizeof(ITEM *));
	for(i = 0; i<4;++i){
		my_items[i]=new_item(menu[i],"");
	}
	my_items[n_choices] = (ITEM *)NULL;
	my_menu = new_menu((ITEM **)my_items);


	initscr();

	stringstream ss;
	const char *s;
	ofstream outFile;	
	outFile.open("output.txt");
	noecho();
	//These next lines print the F1:Menu, Title, and filename to the screen
	printw("F1: Menu");
	char mesg[]="CSCI 1730 Editor!";
	int row, col;
	getmaxyx(stdscr, row, col);
	mvprintw(0,(col-strlen(mesg))/2,"%s", mesg);
	refresh();
	
	//At this point, cursor is at the end of mesg[]

	//The next section creates a window within stdscr, that serves as our border
	//It also creates a smaller window inside the border that serves as our input area.
	WINDOW *borders;	//Make it pretty
	WINDOW *inputs;		//somewhere for the user to type
	WINDOW *win_menu;	//MENU's window
	WINDOW *confirm;	//MENU's confirmation window, aka where it shows "What file do you want to open? etc"
	
	int startx, starty, width, height;
	int currentX;
	int currentY;
	height = LINES/2;
	width = COLS/2;
	starty = (LINES-height)/2;
	startx = (COLS-width)/2;
	borders = create_newwin(height, width, starty, startx);
	inputs = create_newwin(height-2, width-2, starty+1, startx+1);
	wborder(borders, '|', '|', '-', '-', '+', '+', '+', '+');
	wmove(inputs,0,0);
	int fd;
	wrefresh(borders);
	wrefresh(inputs);	
	char myMenu[10]="My menu";	
	//Create window to be associated with menu
	//Print border and print title of menu
	win_menu = newwin(10,40,4,4);
	keypad(win_menu, TRUE);
	set_menu_win(my_menu, inputs);
	set_menu_sub(my_menu, derwin(win_menu, 6, 38, 3, 1));
	set_menu_mark(my_menu, " * ");
		
	//So far we have CREATED and SHOWN WINDOWS: borders, inputs
	//win_menu is not shown yet, but it is "created" not shown yet because box() has not been called yet
	//Cursor is now inside inputs WINDOW, at position 0,0. When we call refresh, it physically moves the cursor.

	//Following code opens up a file
	char filename[100];		
	const char * filename1;
	if(argc==1){	
		//filename?					//If no file name given			
		mvwprintw(inputs, 0,0,"%s", "What is the file you want to open? ");
		echo();
		wrefresh(inputs);
		wgetstr(inputs,filename);
		fd = open(filename, O_RDONLY);	
		s=strerror(errno);
		mvprintw(row-1,0,"%s",filename);
		noecho();
		refresh();
		//filename?vvvv
		while(fd == -1){				//An error occurred	
			wmove(inputs,1,0);			//Prompt user for solutions
			wrefresh(inputs);
			mvwprintw(inputs,2,0,"%s",s);	
			wmove(inputs, 4, 0);
			wrefresh(inputs);
			mvwprintw(inputs, 5,0,"%s","Enter a new filename ");
			echo();
			wgetstr(inputs,filename);
			fd = open(filename, O_RDONLY);	
			s=strerror(errno);
			mvprintw(row-1,0,"%s",filename);
			refresh();
			werase(inputs);
		}
	}else{							//Filename was given in arguments
		filename1 = argv[1];
		fd = open(filename1,O_RDONLY);
		const char * s=strerror(errno);
		mvprintw(row-1,0,"%s", filename1);
		refresh();
		while(fd == -1){				//An error occurred	
			//filename
			wmove(inputs,1,0);			//Prompt user for solutions
			wrefresh(inputs);
			mvwprintw(inputs,2,0,"%s",s);	
			wmove(inputs, 4, 0);
			wrefresh(inputs);
			mvwprintw(inputs, 5,0,"%s","Enter a new filename ");
			echo();
			wgetstr(inputs,filename);
			fd = open(filename, O_RDONLY);	
			s=strerror(errno);
			mvprintw(row-1,0,"%s",filename);
			refresh();
			werase(inputs);
		}
		
	}
	int n;					//No error, opens the file to 'inputs' window
	char buffer [256];
	while((n=read(fd,buffer,256)) > 0){
		if(n!=-1){
			buffer[n] = '\0';
			mvwprintw(inputs,0,0,"%s",buffer);
			outFile<<buffer;
			wrefresh(inputs);
		}
	}
	
	
	//Now we allow the user to enter text.
	keypad(inputs, TRUE);

	int ch;
	char omg;
	//Add char to str somehow
	scrollok(inputs,TRUE);

	while((ch=wgetch(inputs))){
		if(ch==KEY_F(1)){	//Open the menu
			post_menu(my_menu);
			box(win_menu,0,0);
			print_in_middle(win_menu,1,0,40,myMenu,COLOR_PAIR(1));
			mvwaddch(win_menu,2,0,ACS_LTEE);
			mvwhline(win_menu,2,1,ACS_HLINE,38);
       	                mvwaddch(win_menu,2,39,ACS_RTEE);
			wrefresh(win_menu);
			int chh;
			int counter;
			counter=0;
		
			while((chh=wgetch(win_menu))) {		//Get input for the MENU
				if(chh==KEY_DOWN){
					menu_driver(my_menu, REQ_DOWN_ITEM);
					counter++;
					if(counter>3){
						counter=3;
					}
				}//if
				if(chh==KEY_UP){
					menu_driver(my_menu, REQ_UP_ITEM);
       	                                counter--;
					if(counter<0){
						counter=0;
					}
				}//if
				if(chh=='\n'){		
					if(counter==0){		//OPEN
						confirm = create_newwin(8, 40, 20, 40);
						mvwprintw(confirm,1,1,"%s","Name of file to open? ");
						wrefresh(confirm);
						echo();
						wgetstr(confirm,filename);
						fd=open(filename,O_RDONLY);
						s=strerror(errno);
       	                                        mvprintw(row-1,0,"%s",filename);
       	                                        noecho();
						refresh();
						while(fd == -1){		//Error occured in openning
							mvwprintw(confirm,2,1,"%s",s);
							mvwprintw(confirm,3,1,"%s","Enter a new filename ");
							echo();
							wgetstr(confirm, filename);
							fd = open(filename, O_RDONLY);
       	                                                s=strerror(errno);
       	                                           	mvprintw(row-1,0,"%s",filename);
       	                                         	noecho();
							refresh();
						}
						
						int n;
						char buffer [256];
       	                                       	werase(inputs);
						outFile.close();
						outFile.open("output.txt", std::ofstream::out | std::ofstream::trunc);
       	                         		while((n=read(fd,buffer,256)) > 0){
       	                                        	if(n!=-1){
								
       	                                               		buffer[n] = '\0';
       	                                              	        mvwprintw(inputs,0,0,"%s",buffer);
       	                                                        outFile << buffer;
       	                                                        wrefresh(inputs);
       	                                               	}//if
                                                }//while for reading
						counter=0;
						unpost_menu(my_menu);			//DELETE MENU
						
						werase(win_menu);
						wrefresh(win_menu);
						delwin(win_menu);	
						werase(confirm);
						wrefresh(confirm);
						wrefresh(confirm);
						redrawwin(inputs);
						redrawwin(borders);
						wrefresh(borders);
						wrefresh(inputs);
						
					}else if(counter==1){		//SAVE
						char oldname[]="output.txt";
						string str(filename);
						char newname[]="text.txt";
						rename(oldname,newname);
						unpost_menu(my_menu);	
						werase(win_menu);
						wrefresh(win_menu);
						delwin(win_menu);	
							
						free_menu(my_menu);
						for(int k=0;k<n_choices;++k){
							free_item(my_items[k]);
						}
						endwin();
						return EXIT_SUCCESS;
					}else if(counter==2){		//SAVE AS
						char oldname[]="output.txt";
						confirm = create_newwin(8, 40, 50, 80);
						mvwprintw(confirm,1,1,"%s","Name of file to save to? ");
						wrefresh(confirm);
						echo();
						wgetstr(confirm,filename);
						noecho();
						rename(oldname,filename);
						unpost_menu(my_menu);
						free_menu(my_menu);
						for(int k=0;k<n_choices;++k){
							free_item(my_items[k]);
						}
						endwin();
						outFile.close();
						return EXIT_SUCCESS;
					}else if(counter==3){		//EXIT				
						unpost_menu(my_menu);
						free_menu(my_menu);
						for(int k=0;k<n_choices;++k){
							free_item(my_items[k]);
						}
						endwin();
						outFile.close();
						return EXIT_SUCCESS;
					}
					
					break; //BREAKS FROM THE MENU WHILE LOOP
				}//if chh='\n'
			}//while (for the menu)
		}else if(ch==KEY_BACKSPACE){	
								
		}else if(ch==KEY_UP){					//Takes care up arrow keys
			getyx(inputs,currentY,currentX);
			wmove(inputs,currentY-1,currentX);
		}else if(ch==KEY_DOWN){
			getyx(inputs,currentY,currentX);
			wmove(inputs,currentY+1,currentX);
		}else if(ch==KEY_LEFT){
			getyx(inputs,currentY,currentX);
			wmove(inputs,currentY,currentX-1);
		}else if(ch==KEY_RIGHT){
			getyx(inputs,currentY,currentX);
			wmove(inputs,currentY,currentX+1);
		}else{					//No special key was pressed, type to inputs
			wprintw(inputs, "%c", ch);	//PRINT TO TEMP FILE HERE
			wrefresh(inputs);
			omg = ch;
			outFile<<omg;
		}
	}//First while loop (ch for inputs)
	outFile.close();	
	unpost_menu(my_menu);
	free_menu(my_menu);
	for(int k=0;k<n_choices;++k){
		free_item(my_items[k]);
	}
	endwin();
	return EXIT_SUCCESS;
}
