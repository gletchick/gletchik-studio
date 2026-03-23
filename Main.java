import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        System.out.println("Java Echo Program Started.");
        System.out.println("Type something and press Enter (type 'exit' to quit):");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            if (scanner.hasNextLine()) {
                String input = scanner.nextLine();

                if ("exit".equalsIgnoreCase(input.trim())) {
                    System.out.println("Exiting Java program...");
                    break;
                }

                System.out.println("Echo: " + input);
            }
        }

        scanner.close();
    }
}