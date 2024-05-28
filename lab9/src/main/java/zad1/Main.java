package zad1;

import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    public static void main(String[] args) {
        Warehouse warehouse = new Warehouse();

        final int numberOfProducers = 10;
        final int numberOfConsumers = 10;

        ExecutorService producersThreadPool = Executors.newFixedThreadPool(numberOfProducers);
        ExecutorService consumersThreadPool = Executors.newFixedThreadPool(numberOfConsumers);

        for (int i = 0; i < numberOfProducers; i++) {
            producersThreadPool.execute(new Producer(warehouse));
        }

        for (int i = 0; i < numberOfConsumers; i++) {
            consumersThreadPool.execute(new Consumer(warehouse));
        }

        try {
            Scanner s = new Scanner(System.in);
            s.nextLine();
        } catch (Exception e) {
            e.printStackTrace();
        }

        warehouse.stop();
        producersThreadPool.shutdownNow();
        consumersThreadPool.shutdownNow();
    }
}
