


int main(int argc, char **argv)
{
	int j = 10;
	int k;


	clock();
	for(int i = 0; i < 100000; i++){
		k = j % i;
	}	
	clock();

	

	return 0;
}
