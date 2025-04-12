import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

/**
 * A classe GUI é responsável pela interface gráfica do jogo POOTRIVIA.
 * Ela exibe perguntas, opções de resposta, controla as respostas do usuário e apresenta a pontuação final.
 */
public class GUI {
    /**
     * Objeto responsável por carregar perguntas do jogo.
     */
    protected Carrega questao = new Carrega();
    /**
     * Índice da pergunta atual sendo exibida.
     */
    private int perguntaAtual = 0;
    /**
     * Janela principal da interface gráfica.
     */
    JFrame frame = new JFrame("POOTRIVIA");
    /**
     * Pontuação final do jogador.
     */
    private int pontuacaoFinal = 0;
    /**
     * Objeto para registrar o desempenho do jogador.
     */
    private registoJogo registo = new registoJogo();

    /**
     * Método para exibir a interface do jogo.
     */
    public void guiInicial() {
        // Código para exibir a interface inicial
        frame.setSize(500, 500);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        if (perguntaAtual < 2) {
            questao.perguntas.get(perguntaAtual).modoEasy();
        } else {
            questao.perguntas.get(perguntaAtual).modoHard();
        }
        // pergunta
        JLabel perguntaLabel = new JLabel(questao.perguntas.get(perguntaAtual).getPergunta());
        JPanel painel = new JPanel();
        JPanel painelResposta = new JPanel();
        painelResposta.setLayout(new GridLayout(5, 1));
        ArrayList<JButton> botoesDasRespostas = new ArrayList<>();
        for (String respostas : questao.perguntas.get(perguntaAtual).getResposta()) {
            JButton botao = new JButton(respostas);
            botao.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    verificaResposta(botao.getText(), painel);
                }
            });
            botoesDasRespostas.add(botao);
        }
        for (JButton botao : botoesDasRespostas) {
            painelResposta.add(botao);
        }

        painel.setLayout(new BorderLayout());
        painel.add(perguntaLabel, BorderLayout.NORTH);
        painel.add(painelResposta, BorderLayout.CENTER);
        frame.add(painel);
        frame.setVisible(true);


    }

    /**
     * Método para verificar a resposta e prosseguir para a próxima pergunta.
     *
     * @param resposta Resposta selecionada pelo usuário.
     * @param painel  Painel da interface gráfica.
     */

    private void verificaResposta(String resposta, JPanel painel) {
        // Código para verificar a resposta e atualizar a pontuação
        String respostaCorreta = questao.perguntas.get(perguntaAtual).getRespostaCerta();
        if (resposta.equals(respostaCorreta)) {
            questao.perguntas.get(perguntaAtual).majoracao();
            String mensagemCerta = "Resposta Certa: " + questao.perguntas.get(perguntaAtual).getPontuacao() + " pontos";
            pontuacaoFinal = pontuacaoFinal + questao.perguntas.get(perguntaAtual).getPontuacao();
            registo.adicionarPerguntaCerta(questao.perguntas.get(perguntaAtual).getPergunta());
            JOptionPane.showMessageDialog(null, mensagemCerta);
        } else {
            registo.adicionarPerguntaIncorreta(questao.perguntas.get(perguntaAtual).getPergunta());
            JOptionPane.showMessageDialog(null, "Resposta Errada");
        }
        perguntaAtual++;
        if (perguntaAtual < questao.perguntas.size()) {
            frame.remove(painel);
            guiInicial();
        } else {
            frame.remove(painel);
            guiFinal();
        }

    }
    /**
     * Método para exibir a interface final do jogo , permitindo ao jogador reiniciar ou guardar.
     */
    private void guiFinal() {
        // Código para exibir a interface final do jogo
        JLabel pontosFinal = new JLabel("Pontuação final: " + pontuacaoFinal);
        JLabel nome = new JLabel("Nome Completo: ");
        JLabel data = new JLabel("Data: ");
        JTextField nomeTexto = new JTextField(5);
        JTextField dataTexto = new JTextField(5);
        JButton reiniciarBotao = new JButton("Reiniciar");
        JButton guardarBotao = new JButton("Guardar");
        JPanel painelFinal = new JPanel(new BorderLayout());
        JPanel painelTexto = new JPanel(new GridLayout(2, 2));
        painelTexto.add(nome);
        painelTexto.add(nomeTexto);
        painelTexto.add(data);
        painelTexto.add(dataTexto);
        JPanel painelBotoes = new JPanel(new GridLayout(2, 0));
        painelBotoes.add(reiniciarBotao);
        painelBotoes.add(guardarBotao);
        painelFinal.setLayout(new BorderLayout());
        painelFinal.add(pontosFinal, BorderLayout.NORTH);
        painelFinal.add(painelTexto, BorderLayout.CENTER);
        painelFinal.add(painelBotoes, BorderLayout.SOUTH);
        frame.add(painelFinal);
        frame.setVisible(true);
        guardarBotao.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                registo.setNome(nomeTexto.getText());
                registo.setData(dataTexto.getText());
                registo.guardarFicheiro();
                frame.remove(painelFinal);
                frame.dispose();
            }
        });
        reiniciarBotao.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                registo.setNome(nomeTexto.getText());
                registo.setData(dataTexto.getText());
                registo.guardarFicheiro();
                frame.remove(painelFinal);
                reiniciarJogo();
            }
        });

    }

    /**
     * Método para reiniciar o jogo, resetando a pontuação, o índice da pergunta e o registo do jogador.
     */
    private void reiniciarJogo() {
        // Código para reiniciar o jogo
        pontuacaoFinal = 0;
        perguntaAtual = 0;
        registo = new registoJogo();
        guiInicial();
    }
}