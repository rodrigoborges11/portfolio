import java.io.*;
import java.util.ArrayList;
import java.util.List;

/**
 * A classe registoJogo representa o registo do desempenho do jogador durante o jogo POOTRIVIA.
 * Armazena informações como nome, data, perguntas corretas e perguntas incorretas.
 */

public class registoJogo implements Serializable {
    /**
     * nome
     */
    private String nome;
    /**
     * data
     */
    private String data;
    /**
     * Lista de perguntas respondidas corretamente pelo jogador.
     */
    private List<String> perguntasCorretas = new ArrayList<>();
    /**
     * Lista de perguntas respondidas incorretamente pelo jogador.
     */
    private List<String> perguntasIncorretas = new ArrayList<>();
    /**
     * Construtor padrão da classe registoJogo.
     */
    public registoJogo(){
    }

    public void setNome(String nome) {
        this.nome = nome;
    }
    public void setData(String data) {
        this.data = data;
    }
    /**
     * Adiciona uma pergunta correta à lista.
     *
     * @param pergunta A pergunta respondida corretamente.
     */
    public void adicionarPerguntaCerta(String pergunta){
        perguntasCorretas.add(pergunta);
    }
    /**
     * Adiciona uma pergunta incorreta à lista.
     *
     * @param pergunta A pergunta respondida incorretamente.
     */
    public void adicionarPerguntaIncorreta(String pergunta){
        perguntasIncorretas.add(pergunta);
    }

    public void setPerguntasCorretas(List<String> perguntasCorretas) {
        this.perguntasCorretas = perguntasCorretas;
    }

    public void setPerguntasIncorretas(List<String> perguntasIncorretas) {
        this.perguntasIncorretas = perguntasIncorretas;
    }

    /**
     * Salva o registo do jogador num ficheiro.
     */
    public void guardarFicheiro(){
        registoJogo jogador = new registoJogo();
        jogador.setNome(nome);
        jogador.setData(data);
        jogador.setPerguntasCorretas(perguntasCorretas);
        jogador.setPerguntasIncorretas(perguntasIncorretas);
        String[] dataExtenso = data.split("-|/| |:");
        String[] nomeIniciais = nome.split(" ");
        String str = "pootrivia_jogo_";
        for(String i:dataExtenso){
            str = str.concat(i);
        }
        str = str.concat("_");
        for(String j: nomeIniciais){
            str = str.concat(String.valueOf(j.charAt(0)));
        }
        str = str.concat(".dat");
        File f = new File(str);
        try {
            FileOutputStream fos = new FileOutputStream(f);
            ObjectOutputStream oos = new ObjectOutputStream(fos);
            oos.writeObject(jogador);
            System.out.println("Objeto salvo com sucesso.");
        } catch (FileNotFoundException e) {
            System.out.println("Erro a criar ficheiro.");
        } catch (IOException e) {
            System.out.println("Erro a escrever para o ficheiro.");
        }
    }
}
