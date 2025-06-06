/* Variable expansion functions for GNU Make.
Copyright (C) 1988, 1989, 1991, 1992, 1993 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Make is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "make.h"
#include "commands.h"
#include "file.h"
#include "variable.h"

/* The next two describe the variable output buffer.
   This buffer is used to hold the variable-expansion of a line of the
   makefile.  It is made bigger with realloc whenever it is too small.
   variable_buffer_length is the size currently allocated.
   variable_buffer is the address of the buffer.  */

static unsigned int variable_buffer_length;
static char *variable_buffer;

/* Subroutine of variable_expand and friends:
   The text to add is LENGTH chars starting at STRING to the variable_buffer.
   The text is added to the buffer at PTR, and the updated pointer into
   the buffer is returned as the value.  Thus, the value returned by
   each call to variable_buffer_output should be the first argument to
   the following call.  */

char *
variable_buffer_output (ptr, string, length)
     char *ptr, *string;
     unsigned int length;
{
  register unsigned int newlen = length + (ptr - variable_buffer);

  if (newlen > variable_buffer_length)
    {
      unsigned int offset = ptr - variable_buffer;
      variable_buffer_length = (newlen + 100 > 2 * variable_buffer_length
				? newlen + 100
				: 2 * variable_buffer_length);
      variable_buffer = (char *) xrealloc (variable_buffer,
					   variable_buffer_length);
      ptr = variable_buffer + offset;
    }

  bcopy (string, ptr, length);
  return ptr + length;
}

/* Return a pointer to the beginning of the variable buffer.  */

static char *
initialize_variable_output ()
{
  /* If we don't have a variable output buffer yet, get one.  */

  if (variable_buffer == 0)
    {
      variable_buffer_length = 200;
      variable_buffer = (char *) xmalloc (variable_buffer_length);
      variable_buffer[0] = '\0';
    }

  return variable_buffer;
}

/* Recursively expand V.  The returned string is malloc'd.  */

char *
recursively_expand (v)
     register struct variable *v;
{
  char *value;

  if (v->expanding)
    {
      /* Expanding V causes infinite recursion.  Lose.  */
      if (reading_filename == 0)
	fatal ("Recursive variable `%s' references itself (eventually)",
	       v->name);
      else
	makefile_fatal
	  (reading_filename, *reading_lineno_ptr, 
	   "Recursive variable `%s' references itself (eventually)",
	   v->name);
    }

  v->expanding = 1;
  value = allocated_variable_expand (v->value);
  v->expanding = 0;

  return value;
}

/* Expand a simple reference to variable NAME, which LENGTH chars long.  */

#ifdef __GNUC__
__inline
#endif
static char *
reference_variable (o, name, length)
     char *o;
     char *name;
     unsigned int length;
{
  register struct variable *v = lookup_variable (name, length);

  if (v != 0 && *v->value != '\0')
    {
      char *value = (v->recursive ? recursively_expand (v) : v->value);
      o = variable_buffer_output (o, value, strlen (value));
      if (v->recursive)
	free (value);
    }

  return o;
}

/* Scan LINE for variable references and expansion-function calls.
   Build in `variable_buffer' the result of expanding the references and calls.
   Return the address of the resulting string, which is null-terminated
   and is valid only until the next time this function is called.  */

char *
variable_expand (line)
     register char *line;
{
  register struct variable *v;
  register char *p, *o, *p1;

  p = line;
  o = initialize_variable_output ();

  while (1)
    {
      /* Copy all following uninteresting chars all at once to the
         variable output buffer, and skip them.  Uninteresting chars end
	 at the next $ or the end of the input.  */

      p1 = index (p, '$');

      o = variable_buffer_output (o, p, p1 != 0 ? p1 - p : strlen (p) + 1);

      if (p1 == 0)
	break;
      p = p1 + 1;

      /* Dispatch on the char that follows the $.  */

      switch (*p)
	{
	case '$':
	  /* $$ seen means output one $ to the variable output buffer.  */
	  o = variable_buffer_output (o, p, 1);
	  break;

	case '(':
	case '{':
	  /* $(...) or ${...} is the general case of substitution.  */
	  {
	    char openparen = *p;
	    char closeparen = (openparen == '(') ? ')' : '}';
	    register char *beg = p + 1;
	    char *op, *begp;
	    char *end;
	    char *colon = 0;

	    op = o;
	    begp = p;
	    if (handle_function (&op, &begp))
	      {
		o = op;
		p = begp;
		break;
	      }

	    /* Is there a variable reference inside the parens or braces?
	       If so, expand it before expanding the entire reference.  */

	    p1 = index (beg, closeparen);
	    if (p1 != 0)
	      p1 = lindex (beg, p1, '$');
	    if (p1 != 0 && lindex (beg, p1, ':') == 0)
	      {
		/* BEG now points past the opening paren or brace.
		   Count parens or braces until it is matched.  */
		int count = 0;
		for (p = beg; *p != '\0'; ++p)
		  {
		    if (*p == openparen)
		      ++count;
		    else if (*p == closeparen && --count < 0)
		      break;
		    else if (colon == 0 && count == 0 && *p == ':')
		      /* Record where we found a colon, which
			 indicates a substitution reference.
			 We want to expand the text before the
			 reference only.  */
		      colon = p;
		  }
		/* If COUNT is >= 0, there were unmatched opening parens
		   or braces, so we go to the simple case of a variable name
		   such as `$($(a)'.  */
		if (count < 0)
		  {
		    char *name = expand_argument (beg, colon == 0 ? p : colon);
		    unsigned int namelen = strlen (name);
		    if (colon == 0)
		      {
			/* This is a simple reference to the expanded name.  */
			o = reference_variable (o, name, namelen);
			free (name);
			break;
		      }
		    else
		      {
			/* This is a substitution reference to the expanded
			   name.  We replace the pending text with a copy
			   containing the expanded name in place of the
			   original name, and then fall through to
			   the normal substitution reference code below.  */
			unsigned int restlen = strlen (colon) + 1;
			beg = (char *) alloca (namelen + restlen);
			bcopy (name, beg, namelen);
			bcopy (colon, &beg[namelen], restlen);
			/* Point COLON into the new copy.  */
			colon = &beg[namelen];
		      }
		  }
	      }

	    /* This is not a reference to a built-in function and
	       it does not contain any variable references inside.
	       There are several things it could be.  */

	    if (colon == 0)
	      colon = index (beg, ':');
	    if (colon != 0 && lindex (beg, colon, closeparen) == 0)
	      {
		/* This is a substitution reference: $(FOO:A=B).  */
		int count;
		char *subst_beg, *subst_end, *replace_beg, *replace_end;

		v = lookup_variable (beg, colon - beg);

		subst_beg = colon + 1;
		count = 0;
		for (p = subst_beg; *p != '\0'; ++p)
		  {
		    if (*p == openparen)
		      ++count;
		    else if (*p == closeparen)
		      --count;
		    else if (*p == '=' && count <= 0)
		      break;
		  }
		if (count > 0)
		  /* There were unmatched opening parens.  */
		  return initialize_variable_output ();
		subst_end = p;

		replace_beg = p + 1;
		count = 0;
		for (p = replace_beg; *p != '\0'; ++p)
		  {
		    if (*p == openparen)
		      ++count;
		    else if (*p == closeparen && --count < 0)
		      break;
		  }
		if (count > 0)
		  /* There were unmatched opening parens.  */
		  return initialize_variable_output ();
		end = p;
		replace_end = p;

		p = expand_argument (subst_beg, subst_end);
		p1 = expand_argument (replace_beg, replace_end);

		if (v != 0 && *v->value != '\0')
		  {
		    char *value = (v->recursive ? recursively_expand (v)
				   : v->value);
		    char *percent = find_percent (p);
		    if (percent != 0)
		      o = patsubst_expand (o, value, p, p1,
					   percent, (char *) 0);
		    else
		      o = subst_expand (o, value,
					p, p1, strlen (p), strlen (p1),
					0, 1);
		    if (v->recursive)
		      free (value);
		  }

		free (p);
		free (p1);
	      }

	    /* No, this must be an ordinary variable reference.  */
	    else
	      {
		/* Look up the value of the variable.  */
		end = index (beg, closeparen);
		if (end == 0)
		  return initialize_variable_output ();
		o = reference_variable (o, beg, end - beg);
	      }

	    /* Advance p past the variable reference to resume scan.  */
	    p = end;
	  }
	  break;

	case '\0':
	  break;

	default:
	  if (isblank (p[-1]))
	    break;

	  /* A $ followed by a random char is a variable reference:
	     $a is equivalent to $(a).  */
	  {
	    /* We could do the expanding here, but this way
	       avoids code repetition at a small performance cost.  */
	    char name[5];
	    name[0] = '$';
	    name[1] = '(';
	    name[2] = *p;
	    name[3] = ')';
	    name[4] = '\0';
	    p1 = allocated_variable_expand (name);
	    o = variable_buffer_output (o, p1, strlen (p1));
	    free (p1);
	  }

	  break;
	}      

      if (*p == '\0')
	break;
      else
	++p;
    }

  (void) variable_buffer_output (o, "", 1);
  return initialize_variable_output ();
}

/* Expand an argument for an expansion function.
   The text starting at STR and ending at END is variable-expanded
   into a null-terminated string that is returned as the value.
   This is done without clobbering `variable_buffer' or the current
   variable-expansion that is in progress.  */

char *
expand_argument (str, end)
     char *str, *end;
{
  char *tmp;

  if (*end == '\0')
    tmp = str;
  else
    {
      tmp = (char *) alloca (end - str + 1);
      bcopy (str, tmp, end - str);
      tmp[end - str] = '\0';
    }

  return allocated_variable_expand (tmp);
}

/* Expand LINE for FILE.  Error messages refer to the file and line where
   FILE's commands were found.  Expansion uses FILE's variable set list.  */

char *
variable_expand_for_file (line, file)
     char *line;
     register struct file *file;
{
  char *result;
  struct variable_set_list *save;

  if (file == 0)
    return variable_expand (line);

  save = current_variable_set_list;
  current_variable_set_list = file->variables;
  reading_filename = file->cmds->filename;
  reading_lineno_ptr = &file->cmds->lineno;
  result = variable_expand (line);
  current_variable_set_list = save;
  reading_filename = 0;
  reading_lineno_ptr = 0;

  return result;
}

/* Like variable_expand_for_file, but the returned string is malloc'd.
   This function is called a lot.  It wants to be efficient.  */

char *
allocated_variable_expand_for_file (line, file)
     char *line;
     struct file *file;
{
  char *value;

  char *obuf = variable_buffer;
  unsigned int olen = variable_buffer_length;

  variable_buffer = 0;

  value = variable_expand_for_file (line, file);

#if 0
  /* Waste a little memory and save time.  */
  value = xrealloc (value, strlen (value))
#endif

  variable_buffer = obuf;
  variable_buffer_length = olen;

  return value;
}
