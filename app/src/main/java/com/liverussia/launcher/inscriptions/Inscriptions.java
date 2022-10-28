package com.liverussia.launcher.inscriptions;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum Inscriptions {
    BALANCE("Баланс: %s руб.");

    private final String text;

    public static String createBalanceInscription(String balance) {
        return String.format(
                BALANCE.getText(),
                balance
        );
    }
}
