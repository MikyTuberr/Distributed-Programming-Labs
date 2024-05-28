package zad1;

import java.util.concurrent.ThreadLocalRandom;

public class Producer implements Runnable {
    private final Warehouse warehouse;
    public Producer(Warehouse warehouse) {
        this.warehouse = warehouse;
    }

    @Override
    public void run() {
        while (warehouse.getRunning()) {
            final ThreadLocalRandom random = ThreadLocalRandom.current();
            Products product = Products.values()[random.nextInt(Products.values().length)];
            int quantity = random.nextInt(10) + 1;

            try {
                warehouse.addProduct(product, quantity);
                System.out.println(Thread.currentThread().getName() + " produced " + quantity + " of " + product);
            } catch (InterruptedException e) {
                System.out.println("zad1.Producer shutting down...");
                return;
            }

            try {
                Thread.sleep(random.nextInt(1000) + 500);
            } catch (InterruptedException e) {
                System.out.println("zad1.Producer shutting down...");
                return;
            }
        }
        System.out.println("zad1.Producer shutting down...");
    }
}
