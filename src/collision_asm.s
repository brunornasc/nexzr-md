/********************************************************/
/*                                                      */
/* Função: COLLISION_testCollision_asm                  */
/*                                                      */
/* Descrição: Verifica colisão entre duas caixas        */
/*            retângulares (AABB collision detection)   */
/*                                                      */
/* Parâmetros:                                          */
/*   d0.w = ax (posição X da caixa A)                   */
/*   d1.w = ay (posição Y da caixa A)                   */
/*   d2.w = aw (largura da caixa A)                     */
/*   d3.w = ah (altura da caixa A)                      */
/*   d4.w = bx (posição X da caixa B)                   */
/*   d5.w = by (posição Y da caixa B)                   */
/*   d6.w = bw (largura da caixa B)                     */
/*   d7.w = bh (altura da caixa B)                      */
/*                                                      */
/* Retorno:                                             */
/*   d0.b = 0 (falso) ou 1 (verdadeiro)                 */
/*                                                      */
/********************************************************/

.globl COLLISION_testCollision_asm

COLLISION_testCollision_asm:
    /* Salva registradores que serão modificados */
    movem.l d1-d7, -(sp)

    /********************************************************/
    /* Primeira verificação:                                */
    /* ax < bx + bw ?                                       */
    /********************************************************/
    move.w d4, d0                                /* d0 = bx */
    add.w  d6, d0                                /* d0 = bx + bw */
    cmp.w  d0, d0                                /* compara ax (d0.w original) com bx + bw */
    bge    .no_collision

    /********************************************************/
    /* Segunda verificação:                                 */
    /* ax + aw > bx ?                                       */
    /********************************************************/
    move.w  d0, d0          /* Restaura ax para d0 (já está lá) */
    add.w   d2, d0          /* d0 = ax + aw */
    cmp.w   d4, d0          /* Compara ax + aw com bx */
    ble     .no_collision   /* Se ax + aw <= bx, não há colisão */

    /********************************************************/
    /* Terceira verificação:                                */
    /* ay < by + bh ?                                       */
    /********************************************************/
    move.w  d5, d0          /* d0 = by */
    add.w   d7, d0          /* d0 = by + bh */
    cmp.w   d1, d0          /* Compara ay com by + bh */
    bge     .no_collision   /* Se ay >= by + bh, não há colisão */

    /********************************************************/
    /* Quarta verificação:                                  */
    /* ay + ah > by ?                                       */
    /********************************************************/
    move.w  d1, d0          /* d0 = ay */
    add.w   d3, d0          /* d0 = ay + ah */
    cmp.w   d5, d0          /* Compara ay + ah com by */
    ble     .no_collision   /* Se ay + ah <= by, não há colisão */

    /********************************************************/
    /* Se chegou até aqui, as caixas colidiram!             */
    /********************************************************/
    moveq #1, d0
    bra .end

.no_collision:
    moveq #0, d0

.end:
    movem.l (sp)+, d1-d7 /* Restaura registradores */
    rts


