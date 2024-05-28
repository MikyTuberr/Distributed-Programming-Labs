package zad1;

import java.util.concurrent.ThreadLocalRandom;

public class Consumer implements Runnable {
    private final Warehouse warehouse;
    public Consumer(Warehouse warehouse) {
        this.warehouse = warehouse;
    }
    @Override
    public void run() {
        while (warehouse.getRunning()) {
            final ThreadLocalRandom random = ThreadLocalRandom.current();
            Products product = Products.values()[random.nextInt(Products.values().length)];
            int quantity = random.nextInt(5) + 1;

            try {
                int takenQuantity = warehouse.takeProduct(product, quantity);
                System.out.println(Thread.currentThread().getName() + " consumed " + takenQuantity + " of " + product);
            } catch (InterruptedException e) {
                System.out.println("zad1.Consumer shutting down...");
                return;
            }

            try {
                Thread.sleep(random.nextInt(1000) + 500);
            } catch (InterruptedException e) {
                System.out.println("zad1.Consumer shutting down...");
                return;
            }
        }
        System.out.println("zad1.Consumer shutting down...");
    }
}
