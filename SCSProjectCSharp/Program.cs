using System;
using System.Diagnostics;
using System.Threading;

class Program
{
    public static void Main(string[] args)
    {
        Stopwatch stopwatchMemoryAllocation = new Stopwatch();

        //-------------------------Memory Allocation------------------------------------------

        stopwatchMemoryAllocation.Start();
        int[] arrayAllocation = new int[1000000];
        stopwatchMemoryAllocation.Stop();
        long elapsedNanosecondsMemoryAllocation = stopwatchMemoryAllocation.ElapsedTicks * 1000000000L / Stopwatch.Frequency;
        double elapsedMillisecondsMemoryAllocation = (double)elapsedNanosecondsMemoryAllocation / 1000000.0;
        Console.WriteLine("Time for memory access in milliseconds: " + elapsedMillisecondsMemoryAllocation);

        //-------------------------Memory Static Access------------------------------------------

        int arraySizeStaticAccess = 1000000;
        int[] arrayStaticAccess = new int[arraySizeStaticAccess];
        for (int i = 0; i < arraySizeStaticAccess; i++)
        {
            arrayStaticAccess[i] = i;
        }
        Stopwatch stopwatchStaticAccess = new Stopwatch();
        stopwatchStaticAccess.Start();
        for (int i = 0; i < arraySizeStaticAccess; i++)
        {
            int elementStatic = arrayStaticAccess[i];
        }
        stopwatchStaticAccess.Stop();
        long elapsedNanosecondsStaticAccess = stopwatchStaticAccess.ElapsedTicks * 1000000000L / Stopwatch.Frequency;
        double elapsedMillisecondsStaticAccess = (double)elapsedNanosecondsStaticAccess / 1000000.0;
        Console.WriteLine("Static memory access took " + elapsedMillisecondsStaticAccess + " milliseconds.");

        //-------------------------Memory Dynamic Access------------------------------------------

        int arraySizeDynamicAccess = 1000000;
        Stopwatch stopwatchDynamicAccess = new Stopwatch();
        stopwatchDynamicAccess.Start();
        List<int> arrayDynamicAccess = new List<int>();
        for (int i = 0; i < arraySizeDynamicAccess; i++)
        {
            arrayDynamicAccess.Add(i);
        }
        stopwatchDynamicAccess.Stop();
        long elapsedNanosecondsDynamicAccess = stopwatchDynamicAccess.ElapsedTicks * 1000000000L / Stopwatch.Frequency;
        double elapsedMillisecondsDynamicAccess = (double)elapsedNanosecondsDynamicAccess / 1000000.0;
        Console.WriteLine("Dynamic memory access took " + elapsedMillisecondsDynamicAccess + " milliseconds.");

        //-------------------------Thread Creation and Execution------------------------------------------

        int numThreads = 1;
        Stopwatch stopwatchCreateThread = new Stopwatch();
        stopwatchCreateThread.Start();
        Thread[] threads = new Thread[numThreads];
        for (int i = 0; i < numThreads; i++)
        {
            threads[i] = new Thread(() => {});
            threads[i].Start();
        }
        foreach (Thread thread in threads)
        {
            thread.Join();
        }
        long elapsedNanosecondsCreateThread = stopwatchCreateThread.ElapsedTicks * 1000000000L / Stopwatch.Frequency;
        double elapsedMillisecondsCreateThread = (double)elapsedNanosecondsCreateThread / 1000000.0;
        Console.WriteLine("Thread creation and execution took " + elapsedMillisecondsCreateThread + " milliseconds.");

        //-------------------------Thread Context Switching------------------------------------------

        int numThreadContextSwitch = 1;
        Thread[] threadsContextSwitch = new Thread[numThreadContextSwitch];
        ManualResetEvent startSignal = new ManualResetEvent(false);
        ManualResetEvent doneSignal = new ManualResetEvent(false);
        for (int i = 0; i < numThreadContextSwitch; i++)
        {
            threadsContextSwitch[i] = new Thread(() =>
            {
                startSignal.WaitOne();
                doneSignal.Set();
            });
            threadsContextSwitch[i].Start();
        }
        Stopwatch stopwatchContextSwitch = new Stopwatch();
        stopwatchContextSwitch.Start();
        startSignal.Set();
        doneSignal.WaitOne();
        doneSignal.WaitOne();
        stopwatchContextSwitch.Stop();
        long elapsedNanosecondsContextSwitch = stopwatchContextSwitch.ElapsedTicks * 1000000000L / Stopwatch.Frequency;
        double elapsedMillisecondsContextSwitch = (double)elapsedNanosecondsContextSwitch / 1000000.0;
        Console.WriteLine("Execution time for context switching: " + elapsedMillisecondsContextSwitch + " milliseconds.");

        //-------------------------Thread Migration------------------------------------------

        int numThreadsContext = 1;
        CountdownEvent countdownEvent = new CountdownEvent(numThreads);
        for (int i = 0; i < numThreadsContext; i++)
        {
            int threadNumber = i;
            Task.Run(() =>
            {
                long start = Stopwatch.GetTimestamp();
                while (true)
                {
                    Thread.SpinWait(1000);
                    if (Thread.VolatileRead(ref threadNumber) != i)
                        break;
                }
                long end = Stopwatch.GetTimestamp();
                double migrationTimeMs = (end - start) * 1000.0 / Stopwatch.Frequency;
                Console.WriteLine($"Thread {threadNumber} migration time: {migrationTimeMs} milliseconds");
                countdownEvent.Signal();
            });
        }
        countdownEvent.Wait();
        Console.WriteLine("All threads have completed.");
    }
}
