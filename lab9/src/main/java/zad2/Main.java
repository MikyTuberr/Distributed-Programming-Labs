package zad2;

import java.util.Scanner;
import java.util.concurrent.*;

public class Main {

    private static void stopProgram(CachingPrimeChecker checker, ExecutorService threadPool) {
        checker.shutdown();
        threadPool.shutdown();
        try {
            if (!threadPool.awaitTermination(60, TimeUnit.SECONDS)) {
                threadPool.shutdownNow();
            }
        } catch (InterruptedException ex) {
            threadPool.shutdownNow();
            Thread.currentThread().interrupt();
        }
    }
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        CachingPrimeChecker checker = new CachingPrimeChecker();
        final int numberOfThreads = 4;
        ExecutorService threadPool = Executors.newFixedThreadPool(numberOfThreads);

        while (true) {
            long[] numbers = new long[numberOfThreads];
            String[] ordinals = new String[]{"first", "second", "third", "fourth"};
            for (int i = 0; i < numberOfThreads; i++) {
                System.out.printf("Enter %s number (or 'quit' to exit): ", ordinals[i]);
                String input = scanner.next();
                if (input.equalsIgnoreCase("quit")) {
                    stopProgram(checker, threadPool);
                    return;
                }
                numbers[i] = Long.parseLong(input);
            }

            CountDownLatch latch = new CountDownLatch(numberOfThreads);

            for (long number : numbers) {
                threadPool.submit(() -> {
                    try {
                        boolean isPrime = checker.isPrime(number);
                        System.out.printf("Number %d is %s\n", number, isPrime ? "prime" : "not prime");
                    } finally {
                        latch.countDown();
                    }
                });
            }

            try {
                latch.await();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                System.out.println("Main thread interrupted");
                break;
            }
        }

        stopProgram(checker, threadPool);
    }
}
