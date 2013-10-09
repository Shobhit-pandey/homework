# $FreeBSD: src/share/skel/dot.cshrc,v 1.9.2.1 1999/08/29 16:47:50 peter Exp $
# .cshrc - csh resource script, read at beginning 
#	   of execution by each shell
#
# heavily modified by D. K. McGrath
# all initialization is done here
# .login is empty, except for environment specific stuff


#add environment specific settings below!
#######################begin CS specific###############################
#echo $MANPATH

source /usr/local/apps/intel/studio.2011-sp1/bin/compilervars.csh intel64


setenv PATH ${HOME}/bin:/scratch/bin:/usr/bin:${PATH}:/usr/local/apps/bin

#######################End CS specific#################################

########################Version Control################################
setenv    CVS_RSH   ssh
#######################################################################

######################Completions######################################
complete cvs 'p@1@`cvs --help-commands | & awk '"'"'{print $1}'"'"' | grep -v CVS | grep -v Specify`@'
complete svn 'p@1@`svn help | & awk '"'"'{print $1}'"'"' | tail -n 36 | head -n 33`@'
complete icc 'c/-I/d/'
complete icpc 'c/-I/d/'
complete man 'p/*/c/'
#######################################################################


alias h        history 25
alias j        jobs -l
alias f        finger -sh
alias cd       "cd \!* && ls"
alias emacs    "emacs -g 100x80+1600+0 --no-splash \!*"
alias 411      "cd ~/teaching/cs411/11f/"
alias 311      "cd ~/teaching/cs311/12w/"
alias 161      "cd ~/teaching/cs161/12w/"
alias 151      "cd ~/teaching/cs151/12w/"
alias matlab   matlab -nodesktop -nosplash



#to set the title bar of each window on command execution
if( `uname` != "SunOS" ) then
    alias cwdcmd   'echo -n "]2;`whoami`@`hostname -s`:$cwd "'
endif

#lots of environment variables...
#for BASH, use:
#export EDITOR=/usr/bin/emacs
setenv    INTEL_LICENSE_FILE 28518@linlic.engr.oregonstate.edu
setenv    EDITOR       /usr/bin/emacs
setenv    PAGER        less
setenv    BLOCKSIZE    K
setenv    LC_ALL       C
setenv    LESS         " -R "
#gotta have my colors...
setenv    CLICOLOR
setenv    TERM         xterm-color

setenv QTDIR /usr/lib64/qt4
setenv QTINC /usr/lib64/qt4/include
setenv QTLIB /usr/lib64/qt4/lib


if ($?prompt) then
    # An interactive shell -- set some stuff up
    set filec #not strictly needed...
    set history = 1000
    set savehist = (1000 merge)
    set histfile = ~/.tcsh_history
    #set mail = (/var/mail/$USER)
    set complete = enhance
    set addsuffix
    set autolist = ambiguous
    #stupid emacs backup files...ignore them!
    set fignore  = (.o \~)
    set correct = cmd
	set symlinks = ignore
    #magic command scrolling!
    if ( $?tcsh ) then
        bindkey -k up history-search-backward
        bindkey -k down history-search-forward
    endif
    #For possibly offensive fortunes (pick your system):
    #OS X:
    #[ -x /sw/bin/fortune ] && /sw/bin/fortune -a
    #*BSD
    #[ -x /usr/games/fortune ] && /usr/games/fortune
    #Gentoo Linux
    #[ -x /usr/bin/fortune ] && /usr/bin/fortune -a
endif


#so, I like colors in my prompt, but emacs doesn't interpret
#the escape sequences, so set up a prompt without the colors
#if we are running within emacs
if (! $?EMACS) then
    set prompt="%B%{\033[36m%}[%n@%m]%{\033[0m%}%b %{\033[31m%}%C2>%{\033[0m%} "
else
    set prompt="[%n@%m] %C2> "
    unset CLICOLOR
    setenv TERM xterm
    unalias cwdcmd
    setenv PAGER cat
endif 

# A righteous umask
umask 022
#but for the more security minded, comment the above, and uncomment this:
#umask 077

##
# make sure /usr/local/bin is included in the path
# if not, add it in, since we'll likely need it
##


echo $PATH | grep -q -s "/usr/local/bin"
if $status == 1 then
    set path = ( "/usr/local/bin" $path )
endif

#make this the last line in the file, so that nothing else removes it
#on the IU CS linux machines, this is necessary if you want EOF (CTRL-D) 
#to end your shell session

unset autologout
unset ignoreeof

