// @ts-nocheck
/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

@Entry
@Component
struct ContentTable {
  private foodItem: FoodData
  @State fontWeight: FontWeight = FontWeight.Medium
  @Builder IngredientItem(title:string, name: string, value: string) {
    Flex() {
      Text(title)
        .fontSize(17.4)
        .fontWeight(this.fontWeight)
        .layoutWeight(1)
      Flex() {
        Text(name)
          .fontSize(17.4)
          .flexGrow(1)
        Text(value)
          .fontSize(17.4)
      }
      .layoutWeight(2)
    }
    .key('fontWeight')
  }

  build() {
    Flex({ direction: FlexDirection.Column, justifyContent:  FlexAlign.SpaceBetween, alignItems: ItemAlign.Start }) {
      this.IngredientItem('Calories', 'Calories', this.foodItem.calories + 'kcal')
      this.IngredientItem('Nutrition', 'Protein', this.foodItem.protein + 'g')
      this.IngredientItem('', 'Fat', this.foodItem.fat + 'g')
      this.IngredientItem('', 'Carbohydrates', this.foodItem.carbohydrates + 'g')
      this.IngredientItem('', 'VitaminC', this.foodItem.vitaminC + 'mg')
    }
    .height(280)
    .padding({ top: 30, right: 30, left: 30 })
  }
}