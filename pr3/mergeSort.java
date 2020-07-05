import java.util.concurrent.*;

public class mergeSort extends RecursiveAction
{
    static final int SIZE = 500;
    static final int THRESHOLD = 100;

    private int begin;
    private int end;
    private int[] array;

    public mergeSort(int begin, int end, int[] array)
 {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }
    protected void merge(int begin, int end, int[] array) 
{
		int mid = (begin + end) / 2;
		int i = begin;
		int j = mid + 1;
		int k = 0;
		int[] tmp = new int[end - begin + 1];
		while (i <= mid && j <= end)
		 {
			if (array[i]<array[j])   
				tmp[k++] = array[i++];
			else    
				tmp[k++] = array[j++];
		}
		while (j <= end)    
			tmp[k++] = array[j++];
		while (i <= mid)    
			tmp[k++] = array[i++];

		for (i = begin; i <= end; i++)
		 {
			array[i] = tmp[i - begin];
		}
}

@Override
    protected void compute() {
        if (end - begin < THRESHOLD) 
	 {
		for (int i = begin; i < end; i++)
			{
				for(int j=i+1;j<=end;j++)
					if (array[i] > array[j])
					{
						int tmp = array[i];
						array[i] = array[j];
						array[j] = tmp;
					}
			}

         }
        else {
        
            		int mid = (begin + end) / 2;
		mergeSort leftTask = new mergeSort(begin, mid, array);
		mergeSort rightTask = new mergeSort(mid + 1, end, array);

		leftTask.fork();
		rightTask.fork();
		leftTask.join();
		rightTask.join();

		merge(begin, end, array);

        }
    }

	public static void main(String[] args) {
		ForkJoinPool pool = new ForkJoinPool();
		int[] array = new int[SIZE];

		System.out.print("Unsorted array:");
		java.util.Random rand = new java.util.Random();
		for (int i = 0; i < SIZE; i++)
		 {
			array[i] = rand.nextInt(SIZE);
			System.out.print(array[i]+" ");

		}
		
		
		System.out.println("\n\n");

		mergeSort task = new mergeSort(0, SIZE-1, array);

		pool.invoke(task);

		System.out.print("Sorted array:");
		for(int i = 0; i < SIZE; i++)
		{
			System.out.print(array[i]+" ");
		}
		System.out.println("");


	}
}


