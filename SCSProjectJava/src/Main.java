import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicInteger;

public class Main {
    public static void main(String[] args) throws InterruptedException {

        //---------------Memory Allocation--------------------------

        long startTimeAllocation = System.nanoTime();
        List<Integer> arrayAllocation = new ArrayList<>();
        long endTimeAllocation = System.nanoTime();
        long executionTimeAllocation = endTimeAllocation - startTimeAllocation;
        double executionTimeMillisAllocation = (double) executionTimeAllocation / 1_000_000.0;
        System.out.println("Memory allocation took " + executionTimeMillisAllocation + " milliseconds.");

        //---------------Memory Static Access--------------------------

        int arraySizeStaticAccess = 1000000;
        int[] arrayStaticAccess = new int[arraySizeStaticAccess];

        for (int i = 0; i < arraySizeStaticAccess; i++) {
            arrayStaticAccess[i] = i;
        }
        long startTimeStaticAccess = System.nanoTime();
        for (int i = 0; i < arraySizeStaticAccess; i++) {
            int elementStatic = arrayStaticAccess[i];
        }
        long endTimeStaticAccess = System.nanoTime();
        long executionTimeStaticAccess = endTimeStaticAccess - startTimeStaticAccess;
        double executionTimeMillisStaticAccess = (double) executionTimeStaticAccess / 1_000_000.0;

        System.out.println("Static memory access took " + executionTimeMillisStaticAccess + " milliseconds.");

        //---------------Memory Dynamic Access--------------------------

        int arraySizeDynamicAccess = 1000000;
        List<Integer> arrayDynamicAccess = new ArrayList<>();
        for (int i = 0; i < arraySizeDynamicAccess; i++) {
            arrayDynamicAccess.add(i);
        }
        long startTimeDynamicAccess = System.nanoTime();
        for (int i = 0; i < arraySizeDynamicAccess; i++) {
            int element = arrayDynamicAccess.get(i);
        }
        long endTimeDynamicAccess = System.nanoTime();
        long executionTimeDynamicAccess = endTimeDynamicAccess - startTimeDynamicAccess;
        double executionTimeMillisDynamicAccess = (double) executionTimeDynamicAccess / 1_000_000.0;
        System.out.println("Dynamic memory access took " + executionTimeMillisDynamicAccess + " milliseconds.");

        //---------------Thread creation and execution--------------------------

        int numThreads = 1;
        long startTimeCreateThread = System.nanoTime();
        Thread[] threads = new Thread[numThreads];
        for (int i = 0; i < numThreads; i++) {
            threads[i] = new Thread();
            threads[i].start();
        }
        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        long endTimeCreateThread = System.nanoTime();
        long executionTimeCreateThread = endTimeCreateThread - startTimeCreateThread;
        double executionTimeMillisCreateThread = (double) executionTimeCreateThread / 1_000_000.0;
        System.out.println("Thread creation and execution took " + executionTimeMillisCreateThread + " milliseconds.");

        //---------------Thread context switching--------------------------

        int numThreadContextSwitch = 2;
        Thread[] threadsContextSwitch = new Thread[numThreadContextSwitch];
        CountDownLatch startSignal = new CountDownLatch(1); //countdownlatch for threads to wait until a certain event
        CountDownLatch doneSignal = new CountDownLatch(numThreadContextSwitch);
        for (int i = 0; i < numThreadContextSwitch; i++) {
            threadsContextSwitch[i] = new Thread(() -> {
                try {
                    startSignal.await();//current thread wait until doneSignal = 0
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                doneSignal.countDown(); //release all waiting threads when = 0
            });
            threadsContextSwitch[i].start();
        }
        long startTimeContextSwitch = System.nanoTime();
        startSignal.countDown();
        try {
            doneSignal.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        long endTimeContextSwitch = System.nanoTime();
        long elapsedNanosecondsContextSwitch = endTimeContextSwitch - startTimeContextSwitch;
        double elapsedMillisecondsContextSwitch = (double) elapsedNanosecondsContextSwitch / 1_000_000.0;
        System.out.println("Execution time for context switching: " + elapsedMillisecondsContextSwitch + " milliseconds.");

        //---------------Thread migration--------------------------

        int numThreadsMigration = 1;
        AtomicInteger flag = new AtomicInteger(0);
        CountDownLatch countdownEvent = new CountDownLatch(numThreadsMigration);

        for (int i = 0; i < numThreadsMigration; i++) {
            int finalI = i;
            new Thread(() -> {
                long start = System.nanoTime();
                do {
                    try {
                        Thread.sleep(0);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                } while (flag.equals(new AtomicInteger(finalI)));
                long end = System.nanoTime();
                long elapsedNanosecondsMigration = end - start;
                double elapsedMillisecondsMigration = (double) elapsedNanosecondsMigration / 1_000_000.0;
                System.out.println("Thread " + finalI + " migration time: " + elapsedMillisecondsMigration + " milliseconds");
                countdownEvent.countDown();
            }).start();
        }
        countdownEvent.await();
        System.out.println("All threads have completed.");
    }
}