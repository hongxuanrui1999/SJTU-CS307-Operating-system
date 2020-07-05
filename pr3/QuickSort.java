import java.util.concurrent.*;

public class QuickSort extends RecursiveAction
{
    static final int SIZE = 500;
    static final int THRESHOLD = 100;

    private int begin;
    private int end;
    private int[] array;

    public QuickSort(int begin, int end, int[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }
@Override
    protected void compute() {
        if (end - begin < THRESHOLD) 
	  {
            for(int i = end;i>begin;i--)
		{
			int max = 0;
			int max_index = 0;
			for(int j = 0; j <= i; j++)
			{
				if(array[j]>max)
				{
					max_index = j;
					max = array[j];
				}
			}
			array[max_index] = array[i];
			array[i] = max;
		}
         }
        else {
            // divide stage 
            int l = begin, r = end;
		int i = l;
		int j = r;
		int x = array[l];
		while (i < j)
		{
			while (i < j && array[j] >= x) 
				j--;
			if (i < j)
				array[i++] = array[j];
			while (i < j && array[i]< x) 									i++;
			if (i < j)
				array[j--] = array[i];
		}
		array[i] = x;

		QuickSort leftTask = new QuickSort(begin, i - 1, array);
		QuickSort rightTask = new QuickSort(i + 1, end, array);

            leftTask.fork();
            rightTask.fork();

	     leftTask.join();
            rightTask.join();
        }
    }

	public static void main(String[] args) {
		ForkJoinPool pool = new ForkJoinPool();
		int[] array = new int[SIZE];


		java.util.Random rand = new java.util.Random();
		for (int i = 0; i < SIZE; i++) {
			array[i] = rand.nextInt(SIZE);
		}
		System.out.print("Unsorted array:");
		
		for(int i = 0; i < SIZE; i++)
		{
			System.out.print(array[i]+" ");

		}

		System.out.println("\n\n");


		// use fork-join parallelism to sum the array
		QuickSort task = new QuickSort(0, SIZE-1, array);

		pool.invoke(task);

		System.out.print("Sorted array:");
		for(int i = 0; i < SIZE; i++)
		{
			System.out.print(array[i]+" ");
		}
		System.out.println("");

	}
}


