import shlex
import subprocess
import os
import time
import stat


def main():
	pass #don't break on me

def ls_os():
	
	files = os.listdir('.')
	files.extend('. ..'.split())
	sorted_files = []

	for f in files:
		status = os.lstat(f)
		perms = status.st_mode
		uid = status.st_uid
		gid = status.st_gid
		size = status.st_size
		mtime = status.st_mtime
		links = status.st_nlink

		file_tuple = (mtime, perms, uid, gid, size, f, links)
		sorted_files.append(file_tuple)

	sorted_files.sort()

	for f in sorted_files:
		print f[1], f[6], f[2], f[3], '%4s' % f[4], ' '.join(time.ctime(f[0]).split()[1:4]), f[5],
		if os.path.islink(f[5]):
			print '-> ' + os.readlink(f[5])
		else:
			print
		

def ls_sp():
	
	command = 'ls -a -l -t -r -n'

	#use the shlex module to parse
	command_sequence = shlex.split(command)

	ls = subprocess.Popen(command_sequence, stdout=subprocess.PIPE)

	ls_out = ls.stdout.readlines() #bad idea in general

	print ls_out

	for line in ls_out:
		print line,


	

if __name__ == '__main__':
	ls_os()
