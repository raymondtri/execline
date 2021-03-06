/* ISC license. */

#include <string.h>
#include <errno.h>
#include <skalibs/bytestr.h>
#include <skalibs/stralloc.h>
#include <skalibs/types.h>
#include <execline/execline.h>

int el_pushenv (stralloc *sa, char const *const *envp, size_t envlen, char const *const *list, size_t listlen)
{
  size_t i = 0, salen = sa->len ;
  int count = 0 ;
  for (; i < envlen ; i++)
  {
    size_t equal, colon, j = 0 ;
    for (; j < listlen ; j++) if (str_start(envp[i], list[j])) break ;
    if (j == listlen) goto copyit ;
    count++ ;
    j = strlen(list[j]) ;
    colon = j + str_chr(envp[i] + j, ':') ;
    equal = j + str_chr(envp[i] + j, '=') ;
    if (!envp[i][equal]) goto badenv ;
    if (colon >= equal)
    {
      if (!stralloc_catb(sa, envp[i], equal)
       || !stralloc_catb(sa, ":1", 2)) goto err ;
    }
    else
    {
      unsigned int n ;
      char fmt[UINT_FMT+1] = ":" ;
      if (colon + 1 + uint_scan(envp[i] + colon + 1, &n) != equal) goto copyit ;
      j = 1 + uint_fmt(fmt+1, n+1) ;
      if (!stralloc_catb(sa, envp[i], colon)) goto err ;
      if (!stralloc_catb(sa, fmt, j)) goto err ;
    }
    if (!stralloc_catb(sa, envp[i] + equal, strlen(envp[i] + equal) + 1)) goto err ;
    continue ;
copyit:
    if (!stralloc_catb(sa, envp[i], strlen(envp[i]) + 1)) goto err ;
  }
  return count ;

badenv :
  errno = EINVAL ;
err:
  sa->len = salen ;
  return -1 ;
}
