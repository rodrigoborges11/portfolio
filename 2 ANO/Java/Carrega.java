import java.io.*;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.lang.String;
import java.util.Arrays;
/**
 * A classe Carrega é responsável por carregar perguntas a partir de um ficheiro de texto.
 * Esta classe cria uma lista de objetos perguntas.
 */

public class Carrega {
    /**
     * Lista de objetos perguntas.
     */
    protected ArrayList<Perguntas> perguntas = new ArrayList<>();
    /**
     * Construtor da classe Carrega.
     * Carrega a partir de um ficheiro e adiciona a perguntas diferentes objetos.
     */
    public Carrega() {
        File f = new File("D:\\ProjetoPOAO\\pootrivia.txt");
        if (f.exists() && f.isFile()) {
            try {
                FileReader fr = new FileReader(f);
                BufferedReader br = new BufferedReader(fr);
                String linha;
                while ((linha = br.readLine()) != null) {
                    String indices[] = linha.split(";");
                    if (indices.length > 0) {
                        if (indices[0].equals("arte")) {
                            perguntas.add(new Arte(indices[1],indices[2].split(","),indices[3], 5));
                        }
                        if (indices[0].equals("ciencia")) {
                            perguntas.add(new Ciencias(indices[1] ,indices[2].split(","),indices[3].split(","),indices[4], 5));
                        }
                        if (indices[0].equals("ski")) {
                            perguntas.add(new Ski(indices[1] , indices[2].split(","),indices[3],5));
                        }
                        if (indices[0].equals("natacao")) {
                            perguntas.add(new Natacao(indices[1] , indices[2].split(","),indices[3],5));
                        }
                        if (indices[0].equals("futebol")) {
                            perguntas.add(new Futebol(indices[1] , indices[2],indices[3].split(","),indices[4].split(","),indices[5],indices[6], 5));
                        }



                    }
                }
                br.close();
            } catch (FileNotFoundException ex) {
                System.out.println("Erro a abrir ficheiro");
            } catch (IOException ex) {
                System.out.println("Erro a ler ficheiro");
            }
        }

        escolheperguntas();
    }


    /**
     * Método que enquanto o tamanho da lista perguntas for superior a 5 , retirando objetos da lista de modo a ficar 5 , e embaralha-os
     */
    public void escolheperguntas(){
        while (perguntas.size()>5){
            int indice = (int) (Math.random() * perguntas.size());
            perguntas.remove(indice);

        }
        Collections.shuffle(perguntas);
    }
}