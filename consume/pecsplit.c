#include "prefix_map.h"

/** Parsing the variable. */
/* For Applying the variable, and Main program, see prefix_map.h. */

int
unquote (char *src)
{
  for (char *dest = src; 0 != (*dest = *src); ++dest, ++src)
    switch (*src)
      {
      case ':':
      case '=':
	return 0; // should have been escaped
      case '%':
	switch (*(src + 1))
	  {
	  case '.':
	    *dest = ':';
	    goto unquoted;
	  case '+':
	    *dest = '=';
	  unquoted:
	  case '#':
	    ++src;
	    break;
	  default:
	    return 0; // invalid
	  }
      }
  return 1;
}

/* Returns 0 on failure and 1 on success. */
int
parse_prefix_map (char *arg, struct prefix_maps *maps)
{
  char *p;
  p = strchr (arg, '=');
  if (!p)
    return 0;
  *p = '\0';
  if (!unquote (arg))
    return 0;
  p++;
  if (!unquote (p))
    return 0;

  return add_prefix_map (arg, p, maps);
}

/* Returns 0 on failure and 1 on success. */
int
parse_prefix_maps (const char *arg, struct prefix_maps *maps)
{
  size_t len = strlen (arg);
  char *copy = (char *) alloca (len + 1);
  memcpy (copy, arg, len + 1); // strtok modifies the string so we have to copy it

  char *sep = ":", *end;
  char *tok = strtok_r (copy, sep, &end);
  while (tok != NULL)
    {
      if (!parse_prefix_map (tok, maps))
	{
	  fprintf (stderr, "invalid value for prefix-map: '%s'\n", arg);
	  return 0;
	}

      tok = strtok_r (NULL, sep, &end);
    }

  return 1;
}

int
main (int argc, char *argv[])
{
  return generic_main (parse_prefix_maps, argc, argv);
}
