/*
    gcc -lcurses curses.c

    simple curses example; keeps drawing the inputted characters, in columns
    downward, shifting rightward when the last row is reached, and
    wrapping around when the last column is reached
*/

#include <curses.h>

int r, c,            /* current row and column */
    nrows,           /* number of rows in window */
    ncols;           /* number of columns in window */

void draw(char dc)

{  move(r, c);           /* move cursor to row r, column c */
   delch();  insch(dc);  /* replace character under cursor by dc */
   refresh();            /* udate screen */
   r++;                  /* go to next row */
   /* check for need to shift right or wrap around */
   if (r == nrows)  {
      r = 0;
      c++;
      if (c == ncols) c = 0;
   }
}

int main(void)
{
   char d;
   WINDOW *wnd;

   wnd = initscr();             /* initialize window */
   cbreak();                    /* no waiting for Enter key */
   noecho();                    /* no echoing */
   getmaxyx(wnd, nrows, ncols); /* find size of window */
   clear();                     /* clear screen,send cursor to position(0,0)*/
   refresh();                   /* implement all changes since last refresh */

   r = 0; c = 0;
   while (1)  {
      d = getch();             /* input from keyboard */
      if (d == 'q') break;     /* quit? */
      draw(d);                 /* draw the character */
   }

   endwin();                   /* restore the original window */
   return 0;
}
