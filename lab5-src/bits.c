

// It prints the bits in bitmap as 0s and 1s.
void printBits(unsigned int bitmap)
{
	// Add your code here
	int i;
	unsigned int bitname;
	for(i = 31; i >= 0 && (bitname = (1 << i)); i--)
		(bitmap & bitname) ? putchar('1') : putchar('0');
	printf("\n10987654321098765432109876543210\n");
}


// Sets the ith bit in *bitmap with the value in bitValue (0 or 1)
void setBitAt( unsigned int *bitmap, int i, int bitValue ) {
	// Add your code here
	unsigned int bitname = (1 << i);
	(bitValue) ? (*bitmap = ((*bitmap) | bitname)) : (*bitmap = ((*bitmap) & (~bitmask)));
}

// It returns the bit value (0 or 1) at bit i
int getBitAt( unsigned int bitmap, unsigned int i) {
	// Add your code here
	unsigned int bitname = (1 << i);
	return (bitname & bitmap) ? 1 : 0;
}

// It returns the number of bits with a value "bitValue".
// if bitValue is 0, it returns the number of 0s. If bitValue is 1, it returns the number of 1s.
int countBits( unsigned int bitmap, unsigned int bitValue) {
	// Add your code here
	unsigned int bitname;
	int count , countAns , i;
	for(i=0 , countAns=0 , count=0; (i < 32) && (bitname = (1 << i)); i++)
		(bitmap & bitname) ? countAns++ : count++;
	return (bitValue) ? countAns:count;
countAns
}

// It returns the number of largest consecutive 1s in "bitmap".
// Set "*position" to the beginning bit index of the sequence.
int maxContinuousOnes(unsigned int bitmap, int * position) {
	// Add your code here
	int arr[40][2],i,j,in,count,max;
	unsigned int bitname=1;
	for(i=0 , j=0 , in=0 , count=0 , max=0;(i < 32) && (bitname = (1 << i)); i++){
	  if(bitname & bitmap){
			count++;
	  }
	  else {
		  if(count > 0){
		    arr[j][0] = count;
		    arr[j++][1] = i - count;
		  }
		  count=0;
		}	
	}
	for(i = 0;i < j;i++)
		if(arr[i][0] > arr[max][0])
			max = i;
	*position = arr[max][1];
	return arr[max][0];
}


